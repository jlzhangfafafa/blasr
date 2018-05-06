#include <iostream>

#include <ChangeListID.hpp>
#include <CommandLineParser.hpp>
#include <FASTAReader.hpp>
#include <FASTASequence.hpp>
#include <HDFCmpFile.hpp>
#include <datastructures/alignment/AlignmentCandidate.hpp>
#include <datastructures/alignment/SAMToAlignmentCandidateAdapter.hpp>
#include <datastructures/alignmentset/AlignmentSetToCmpH5Adapter.hpp>
#include <format/StickAlignmentPrinter.hpp>
#include <sam/SAMReader.hpp>
#include <utils/TimeUtils.hpp>

char VERSION[] = "v1.0.0";
char PERFORCE_VERSION_STRING[] = "$Change: 141782 $";

int main(int argc, char* argv[])
{
    std::string program = "samtoh5";
    std::string versionString = VERSION;
    AppendPerforceChangelist(PERFORCE_VERSION_STRING, versionString);
    std::string samFileName, cmpFileName, refFileName;
    bool parseSmrtTitle = false;
    bool useShortRefName = false;
    bool copyQVs = false;
    CommandLineParser clp;
    std::string readType = "standard";
    int verbosity = 0;

    clp.SetProgramName(program);
    clp.SetProgramSummary("Converts in.sam file to out.cmp.h5 file.");
    clp.SetVersion(versionString);

    clp.RegisterStringOption("in.sam", &samFileName, "Input SAM file.", true);
    clp.RegisterStringOption("reference.fasta", &refFileName, "Reference used to generate reads.",
                             true);
    clp.RegisterStringOption("out.cmp.h5", &cmpFileName, "Output cmp.h5 file.", true);
    clp.RegisterPreviousFlagsAsHidden();
    clp.RegisterFlagOption("smrtTitle", &parseSmrtTitle,
                           "Use this option when converting alignments "
                           "generated from reads produced by the "
                           "pls2fasta from bas.h5 files by parsing read "
                           "coordinates from the SMRT read title.  The title "
                           "is in the format /name/hole/coordinates, where "
                           "coordinates are in the format \\d+_\\d+, and "
                           "represent the interval of the read that was "
                           "aligned.");
    clp.RegisterStringOption("readType", &readType,
                             "Set the read type: 'standard', 'strobe', 'CCS', "
                             "or 'cDNA'");
    clp.RegisterIntOption("verbosity", &verbosity, "Set desired verbosity.",
                          CommandLineParser::PositiveInteger);
    clp.RegisterFlagOption("useShortRefName", &useShortRefName,
                           "Use abbreviated reference names obtained "
                           "from file.sam instead of using full names "
                           "from reference.fasta.");
    clp.RegisterFlagOption("copyQVs", &copyQVs,
                           "Copy all QVs available in the SAM file into the "
                           "cmp.h5 file. This includes things like InsertionQV "
                           "and DeletionTag.");
    std::string description =
        ("Because SAM has optional tags that have different "
         "meanings in different programs, careful usage is required in order to "
         "have proper output. The \"xs\" tag in bwa-sw is used to show the "
         "suboptimal score, but in PacBio SAM (blasr) it is defined as the start "
         "in the query sequence of the alignment.\nWhen \"-smrtTitle\" is "
         "specified, the xs tag is ignored, but when it is not specified, the "
         "coordinates given by the xs and xe tags are used to define the interval "
         "of a read that is aligned. The CIGAR string is relative to this interval.");
    clp.SetExamples(description);

    clp.ParseCommandLine(argc, argv);

    if (readType != "standard" and readType != "strobe" and readType != "cDNA" and
        readType != "CCS") {
        std::cout << "ERROR. Read type '" << readType
                  << "' must be one of either 'standard', 'strobe', 'cDNA' or 'CCS'." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cerr << "[INFO] " << GetTimestamp() << " [" << program << "] started." << std::endl;

    SAMReader<SAMFullReferenceSequence, SAMReadGroup, SAMPosAlignment> samReader;
    FASTAReader fastaReader;
    HDFCmpFile<AlignmentCandidate<FASTASequence, FASTASequence> > cmpFile;

    //
    // Initialize input/output files.
    //
    samReader.Initialize(samFileName);
    fastaReader.Initialize(refFileName);
    cmpFile.Create(cmpFileName);

    //
    // Configure the file log.
    //
    std::string command;
    CommandLineParser::CommandLineToString(argc, argv, command);
    std::string log = "Convert sam to cmp.h5";
    cmpFile.fileLogGroup.AddEntry(command, log, program, GetTimestamp(), versionString);

    //
    // Set the readType
    //
    cmpFile.SetReadType(readType);

    //
    // Read necessary input.
    //

    std::vector<FASTASequence> references;
    fastaReader.ReadAllSequences(references);

    //
    // This should probably be handled by the alignmentSetAdapter, but
    // time constraints...
    //
    AlignmentSet<SAMFullReferenceSequence, SAMReadGroup, SAMPosAlignment> alignmentSet;
    samReader.ReadHeader(alignmentSet);

    //
    // The order of references in std::vector<FASTASequence> references and
    // AlignmentSet<, , >alignmentSet.references can be different.
    // Rearrange alignmentSet.references such that it is ordered in
    // exactly the same way as std::vector<FASTASequence> references.
    //
    alignmentSet.RearrangeReferences(references);

    //
    // Always recompute the MD5 values even if they exist in the input
    // sam file. Because MD5 is defined differently in sam and cmp.h5 files.
    // The SAM convention uppercases and normalizes before computing the MD5.
    // For cmp.h5, we compute the MD5 on the sequence 'as is'.
    //
    for (size_t i = 0; i < alignmentSet.references.size(); i++) {
        MakeMD5((const char*)&references[i].seq[0], (unsigned int)references[i].length,
                alignmentSet.references[i].md5);
    }

    //
    // Map short names for references obtained from file.sam to full names obtained from reference.fasta
    //
    std::map<std::string, std::string> shortRefNameToFull;
    std::map<std::string, std::string>::iterator it;
    assert(references.size() == alignmentSet.references.size());
    if (!useShortRefName) {
        for (size_t i = 0; i < references.size(); i++) {
            std::string shortRefName = alignmentSet.references[i].GetSequenceName();
            std::string fullRefName(references[i].title);
            if (shortRefNameToFull.find(shortRefName) != shortRefNameToFull.end()) {
                std::cout << "ERROR, Found more than one reference " << shortRefName
                          << "in sam header" << std::endl;
                std::exit(EXIT_FAILURE);
            }
            shortRefNameToFull[shortRefName] = fullRefName;
            alignmentSet.references[i].sequenceName = fullRefName;
        }
    }

    //
    // Start setting up the cmp.h5 file.
    //
    AlignmentSetToCmpH5Adapter<HDFCmpFile<AlignmentCandidate<FASTASequence, FASTASequence> > >
        alignmentSetAdapter;
    alignmentSetAdapter.Initialize();
    alignmentSetAdapter.StoreReferenceInfo(alignmentSet.references, cmpFile);

    //
    // Store the alignments.
    //
    SAMAlignment samAlignment;
    while (samReader.GetNextAlignment(samAlignment)) {
        if (samAlignment.rName == "*") {
            continue;
        }
        if (!useShortRefName) {
            //convert shortRefName to fullRefName
            it = shortRefNameToFull.find(samAlignment.rName);
            if (it == shortRefNameToFull.end()) {
                std::cout << "ERROR, Could not find " << samAlignment.rName
                          << " in the reference repository." << std::endl;
                std::exit(EXIT_FAILURE);
            }
            samAlignment.rName = (*it).second;
        }
        std::vector<AlignmentCandidate<> > convertedAlignments;
        if (verbosity > 0) {
            std::cout << "Storing alignment for " << samAlignment.qName << std::endl;
        }
        SAMAlignmentsToCandidates(samAlignment,
                                  // Order of references and alignmentSetAdapter.RefInfoGroup
                                  // should be exactly the same.
                                  references, alignmentSetAdapter.refNameToRefInfoIndex,
                                  convertedAlignments, parseSmrtTitle, false, copyQVs);

        // -1: moleculeID will be computed dynamically.
        // o.w., the value will be assigned as moleculeID.
        alignmentSetAdapter.StoreAlignmentCandidateList(convertedAlignments, cmpFile, -1, copyQVs);

        for (size_t a = 0; a < convertedAlignments.size(); a++) {
            convertedAlignments[a].FreeSubsequences();
        }
    }

    std::cerr << "[INFO] " << GetTimestamp() << " [" << program << "] ended." << std::endl;
    return 0;
}
