#include <sstream>
#include <string>

#include <alignment/statistics/StatUtils.hpp>
#include <pbdata/CommandLineParser.hpp>
#include <pbdata/FASTAReader.hpp>
#include <pbdata/FASTASequence.hpp>
#include <pbdata/FASTQSequence.hpp>
#include <pbdata/metagenome/FindRandomSequence.hpp>
#include <pbdata/utils.hpp>

int main(int argc, char* argv[])
{
    std::string inFileName, readsFileName;
    DNALength readLength;
    float coverage = 0;
    bool noRandInit = false;
    int numReads = -1;
    CommandLineParser clp;
    int qualityValue = 20;
    bool printFastq = false;
    int stratify = 0;
    std::string titleType = "pacbio";
    std::string fastqType = "illumina";  // or "sanger"
    clp.RegisterStringOption("inFile", &inFileName, "Reference sequence", 0);
    clp.RegisterPreviousFlagsAsHidden();
    clp.RegisterIntOption("readLength", (int*)&readLength,
                          "The length of reads to simulate.  The length is fixed.",
                          CommandLineParser::PositiveInteger, 0);
    clp.RegisterFloatOption("coverage", &coverage,
                            "Total coverage (from which the number of reads is calculated",
                            CommandLineParser::PositiveFloat, 0);
    clp.RegisterFlagOption("nonRandInit", &noRandInit,
                           "Skip initializing the random number generator with time.");
    clp.RegisterIntOption("nReads", &numReads,
                          "Total number of reads (from which coverage is calculated)",
                          CommandLineParser::PositiveInteger, 0);
    clp.RegisterStringOption("readsFile", &readsFileName, "Reads output file", 0);
    clp.RegisterFlagOption("fastq", &printFastq,
                           "Fake fastq output with constant quality value (20)");
    clp.RegisterIntOption("quality", &qualityValue, "Value to use for fastq quality",
                          CommandLineParser::PositiveInteger);
    clp.RegisterIntOption("stratify", &stratify,
                          "Sample a read every 'stratify' bases, rather than randomly.",
                          CommandLineParser::PositiveInteger);
    clp.RegisterStringOption("titleType", &titleType,
                             "Set the name of the title: 'pacbio'|'illumina'");
    clp.RegisterStringOption("fastqType", &fastqType, "Set the type of fastq: 'illumina'|'sanger'");
    std::vector<std::string> leftovers;
    clp.ParseCommandLine(argc, argv, leftovers);

    if (!noRandInit) {
        InitializeRandomGeneratorWithTime();
    }

    FASTAReader inReader;
    inReader.Init(inFileName);
    std::vector<FASTASequence> reference;

    inReader.ReadAllSequences(reference);
    std::ofstream readsFile;
    if (readsFileName == "") {
        std::cout << "ERROR.  You must specify a reads file." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    CrucialOpen(readsFileName, readsFile, std::ios::out);

    std::ofstream sangerFastqFile;
    if (fastqType == "sanger") {
        std::string sangerFastqFileName = readsFileName + ".fastq";
        CrucialOpen(sangerFastqFileName, sangerFastqFile, std::ios::out);
    }

    DNALength refLength = 0;
    for (size_t i = 0; i < reference.size(); i++) {
        refLength += reference[i].length;
    }
    if (numReads == -1 and coverage == 0 and stratify == 0) {
        std::cout << "ERROR, you must specify either coverage, nReads, or stratify." << std::endl;
        std::exit(EXIT_FAILURE);
    } else if (numReads == -1) {
        numReads = (refLength / readLength) * coverage;
    }

    if (stratify) {
        if (!readLength) {
            std::cout << "ERROR. If you are using stratification, a read length must be specified."
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    DNASequence sampleSeq;
    sampleSeq.length = readLength;
    int maxRetry = 10000000;
    int retryNumber = 0;
    DNALength seqIndex, seqPos;
    if (stratify) {
        seqIndex = 0;
        seqPos = 0;
    }
    DNALength origReadLength = readLength;
    for (int i = 0; stratify or i < numReads; i++) {
        if (stratify == 0) {
            FindRandomPos(reference, seqIndex, seqPos, readLength);
        } else {
            //
            // find the next start pos, or bail if done
            //
            if (seqPos >= reference[seqIndex].length) {
                if (seqIndex == reference.size() - 1) {
                    break;
                } else {
                    seqIndex = seqIndex + 1;
                    seqPos = 0;
                    continue;
                }
            }
            readLength = std::min(reference[seqIndex].length - seqPos, origReadLength);
        }
        sampleSeq.seq = &reference[seqIndex].seq[seqPos];
        int j;
        int gappedRead = 0;
        std::string title;
        std::stringstream titleStrm;
        if (titleType == "pacbio") {
            titleStrm << i << "|" << reference[seqIndex].GetName() << "|" << seqPos << "|"
                      << seqPos + readLength;
        } else if (titleType == "illumina") {
            titleStrm << "SE_" << i << "_0@" << seqPos << "-" << seqPos + readLength << "/1";
        } else {
            std::cout << "ERROR. Bad title type " << titleType << std::endl;
            std::exit(EXIT_FAILURE);
        }
        title = titleStrm.str();
        sampleSeq.length = readLength;
        if (!printFastq) {
            readsFile << ">" << title << std::endl;
            sampleSeq.PrintSeq(readsFile);
        } else {
            FASTQSequence fastqSampleSeq;
            fastqSampleSeq.CopyTitle(title);
            fastqSampleSeq.seq = sampleSeq.seq;
            fastqSampleSeq.length = sampleSeq.length;
            fastqSampleSeq.qual.data = new unsigned char[sampleSeq.length];
            std::fill(fastqSampleSeq.qual.data, fastqSampleSeq.qual.data + sampleSeq.length,
                      qualityValue);
            if (fastqType == "illumina") {
                fastqSampleSeq.PrintFastq(readsFile, fastqSampleSeq.length + 1);
            } else {
                fastqSampleSeq.PrintSeq(readsFile);
                fastqSampleSeq.PrintQual(sangerFastqFile);
            }
            delete[] fastqSampleSeq.qual.data;
            delete[] fastqSampleSeq.title;
        }

        if (stratify) {
            seqPos += readLength;
        }
    }
    return 0;
}
