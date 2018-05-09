#include <alignment/datastructures/alignment/CmpFile.hpp>
#include <alignment/files/ReaderAgglomerate.hpp>
#include <alignment/format/StickAlignmentPrinter.hpp>
#include <alignment/simulator/ContextSet.hpp>
#include <alignment/simulator/OutputSampleListSet.hpp>
#include <alignment/utils/FileOfFileNames.hpp>
#include <hdf/HDFCmpFile.hpp>
#include <pbdata/SMRTSequence.hpp>

class ScoredLength
{
public:
    int score, length;
    int operator<(const ScoredLength& rhs) const { return score < rhs.score; }
    ScoredLength(int s, int l) : score(s), length(l) {}
    ScoredLength() {}
};

void PrintUsage()
{
    std::cout << "cmpH5StoreQualityByContext - grab quality values from cmp.h5 files until minimum "
                 "requirements for the number of times a context has been sampled are met."
              << std::endl;
    std::cout << "usage: cmpH5StoreQualityByContext aligned_reads.cmp.h5  output.qbc  [options] "
              << std::endl;
    std::cout << "options: " << std::endl
              << " -contextLength L   The length of the context to sample (default: 5) "
              << std::endl
              << " -minSamples S(500) Report pass if all contexts are sampled" << std::endl
              << "                    at least S times." << std::endl
              << " -maxSamples S(1000) Stop sampling a context once it has reached" << std::endl
              << "                    S samples." << std::endl
              << " -onlyMaxLength" << std::endl
              << "                     Whether or not to store only the length of the" << std::endl
              << "                     longest subread as part of the length model." << std::endl
              << std::endl;
}

int main(int argc, char* argv[])
{
    std::string outFileName;
    unsigned contextLength = 5;
    int minSamples = 500;
    int maxSamples = 1000;
    if (argc < 3) {
        PrintUsage();
        std::exit(EXIT_FAILURE);
    }

    int argi = 1;
    std::string cmpH5FileName;
    cmpH5FileName = argv[argi++];
    outFileName = argv[argi++];
    int minAverageQual = 0;
    bool onlyMaxLength = false;

    while (argi < argc) {
        if (strcmp(argv[argi], "-contextLength") == 0) {
            contextLength = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-minSamples") == 0) {
            minSamples = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-maxSamples") == 0) {
            maxSamples = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-onlyMaxLength") == 0) {
            onlyMaxLength = true;
        } else {
            PrintUsage();
            std::cout << "ERROR, bad option: " << argv[argi] << std::endl;
            std::exit(EXIT_FAILURE);
        }
        ++argi;
    }
    std::map<std::string, ScoredLength> maxLengthMap;
    OutputSampleListSet samples(contextLength);
    SMRTSequence read;

    std::ofstream sampleOut;
    CrucialOpen(outFileName, sampleOut, std::ios::out | std::ios::binary);
    int fileNameIndex;

    int numContextsReached = 0;
    int numContexts = 1 << (contextLength * 2);
    ReaderAgglomerate reader;
    samples.keyLength = contextLength;
    HDFCmpFile<CmpAlignment> cmpReader;
    cmpReader.IncludeField("QualityValue");
    cmpReader.IncludeField("DeletionQV");
    cmpReader.IncludeField("InsertionQV");
    cmpReader.IncludeField("SubstitutionQV");
    cmpReader.IncludeField("SubstitutionTag");
    cmpReader.IncludeField("DeletionTag");
    cmpReader.IncludeField("PulseIndex");
    cmpReader.IncludeField("WidthInFrames");
    cmpReader.IncludeField("PreBaseFrames");

    if (cmpReader.Initialize(cmpH5FileName, H5F_ACC_RDWR) == 0) {
        std::cout << "ERROR, could not open the cmp file." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::cout << "Reading cmp file." << std::endl;

    CmpFile cmpFile;

    cmpReader.ReadAlignmentDescriptions(cmpFile);
    cmpReader.ReadStructure(cmpFile);
    std::cout << "done reading structure." << std::endl;
    int alignmentIndex;
    int nAlignments = cmpReader.alnInfoGroup.GetNAlignments();
    std::vector<int> alignmentToBaseMap;

    for (alignmentIndex = 0; alignmentIndex < nAlignments and !samples.Sufficient();
         alignmentIndex++) {
        //
        // For ease of use, store the length of the alignment to make another model.
        //

        ByteAlignment alignmentArray;
        cmpReader.ReadAlignmentArray(alignmentIndex, alignmentArray);
        Alignment alignment;
        ByteAlignmentToAlignment(alignmentArray, alignment);
        std::string readSequence, refSequence;
        readSequence.resize(alignmentArray.size());
        refSequence.resize(alignmentArray.size());
        DNASequence readDNA, refDNA;

        ByteAlignmentToQueryString(&alignmentArray[0], alignmentArray.size(), &readSequence[0]);
        ByteAlignmentToRefString(&alignmentArray[0], alignmentArray.size(), &refSequence[0]);
        RemoveGaps(readSequence, readSequence);
        RemoveGaps(refSequence, refSequence);

        readDNA.seq = (Nucleotide*)readSequence.c_str();
        readDNA.length = readSequence.size();
        refDNA.seq = (Nucleotide*)refSequence.c_str();
        refDNA.length = refSequence.size();
        CmpAlignment cmpAlignment;

        cmpReader.ImportReadFromCmpH5(alignmentIndex, cmpAlignment, read);

        CreateAlignmentToSequenceMap(alignmentArray, alignmentToBaseMap);

        if (read.length < contextLength) {
            continue;
        }
        int subreadLength = (cmpFile.alnInfo.alignments[alignmentIndex].GetQueryEnd() -
                             cmpFile.alnInfo.alignments[alignmentIndex].GetQueryStart());
        if (onlyMaxLength == false) {
            samples.lengths.push_back(subreadLength);
        } else {
            int score = (cmpAlignment.GetNMatch() - cmpAlignment.GetNMismatch() -
                         cmpAlignment.GetNInsertions() - cmpAlignment.GetNDeletions());
            std::stringstream nameStrm;
            nameStrm << cmpAlignment.GetMovieId() << "_" << cmpAlignment.GetHoleNumber();
            std::string nameStr = nameStrm.str();
            if (maxLengthMap.find(nameStr) == maxLengthMap.end()) {
                maxLengthMap[nameStr] = ScoredLength(score, subreadLength);
            }
        }

        int sampleEnd = alignmentArray.size() - contextLength / 2;
        int a;
        for (a = contextLength / 2; a < sampleEnd; a++) {

            // Make sure the context begins on a real nucleotide.
            while (a < sampleEnd and ((RefChar[alignmentArray[a]] == ' '))) {
                a++;
            }

            //
            // Move ab back to an index where there are contextLength/2 non-gap
            // characters, counted by nb
            //
            int ab;  //num bases
            int ae;  //alignment end
            ab = a - 1;
            int nb = 0, ne = 0;
            while (true) {
                if (RefChar[alignmentArray[ab]] != ' ') {
                    nb++;
                }
                if (ab == 0 or nb == static_cast<int>(contextLength) / 2) break;
                ab--;
            }

            //
            // Advance ae to an index where there are contextLength/2 non-gap
            // characters, counted by ne.
            //
            ae = a + 1;
            while (ae < static_cast<int>(alignmentArray.size()) and
                   ne < static_cast<int>(contextLength) / 2) {
                if (RefChar[alignmentArray[ae]] != ' ') {
                    ne++;
                }
                ae++;
            }

            //
            // Make sure there are no edge effects that prevent a context of the correct length from being assigned.
            //
            if (nb + ne + 1 != static_cast<int>(contextLength)) {
                continue;
            }
            int ai;
            std::string context;
            for (ai = ab; ai < ae; ai++) {
                if (RefChar[alignmentArray[ai]] != ' ') {
                    context.push_back(RefChar[alignmentArray[ai]]);
                }
            }
            assert(context.size() == contextLength);
            //
            // Now create the context.
            //
            OutputSample sample;

            //
            // This context is a deletion, create that.
            //
            sample.type = OutputSample::Deletion;

            //
            // This context is either an insertion or substitution
            //
            // Look to see if the previous aligned position was an
            // insertion, and move back as far as the insertion extends.
            int aq = a - 1;
            int sampleLength;

            if (QueryChar[alignmentArray[a]] == ' ') {
                sample.type = OutputSample::Deletion;
                sampleLength = 0;
            } else if (RefChar[alignmentArray[aq]] == ' ') {

                while (aq > 0 and RefChar[alignmentArray[aq]] == ' ' and
                       QueryChar[alignmentArray[aq]] != ' ') {
                    aq--;
                }
                sample.type = OutputSample::Insertion;
                sampleLength = a - aq;
            } else if (QueryChar[alignmentArray[a]] == RefChar[alignmentArray[aq]]) {
                sample.type = OutputSample::Match;
                sampleLength = 1;
            } else {
                sample.type = OutputSample::Substitution;
                sampleLength = 1;
            }

            sample.Resize(sampleLength);
            if (sampleLength > 0) {
                int seqPos = alignmentToBaseMap[aq];
                if (seqPos < static_cast<int>(read.length)) {
                    sample.CopyFromSeq(read, seqPos, sampleLength);
                    std::string nucs;
                    for (size_t n = 0; n < sample.nucleotides.size(); n++) {
                        char c = sample.nucleotides[n];
                        assert(c == 'A' or c == 'T' or c == 'G' or c == 'C');
                        nucs.push_back(sample.nucleotides[n]);
                    }
                }
            }
            samples.AppendOutputSample(context, sample);
        }
        read.Free();
    }

    if (onlyMaxLength) {
        std::map<std::string, ScoredLength>::iterator maxScoreIt;
        for (maxScoreIt = maxLengthMap.begin(); maxScoreIt != maxLengthMap.end(); ++maxScoreIt) {
            std::cout << maxScoreIt->second.length << std::endl;
            samples.lengths.push_back(maxScoreIt->second.length);
        }
    }

    samples.Write(sampleOut);

    return 0;
}
