#include <sstream>
#include <vector>

#include <alignment/statistics/StatUtils.hpp>
#include <pbdata/CommandLineParser.hpp>
#include <pbdata/FASTAReader.hpp>
#include <pbdata/FASTASequence.hpp>
#include <pbdata/utils.hpp>

/*
ref000001	.	SNV	9454	9454	0.00	.	.	reference=C;confidence=0;Name=9454C>A;coverage=0;variantseq=A
ref000001	.	deletion	20223	20223	0.00	.	.	reference=T;length=1;confidence=0;coverage=0;Name=20222delT
ref000001	.	insertion	35089	35089	0.00	.	.	confidence=0;Name=35089_35090insC;reference=.;length=1;coverage=0;variantseq=C
*/

char ToLower(char c, bool useToLower)
{
    if (useToLower) {
        return tolower(c);
    } else {
        return toupper(c);
    }
}

int main(int argc, char* argv[])
{
    CommandLineParser clp;

    std::string refGenomeName;
    std::string mutGenomeName;
    std::string gffFileName;
    float insRate = 0;
    float delRate = 0;
    float mutRate = 0;
    bool lower = false;
    gffFileName = "";
    clp.RegisterStringOption("refGenome", &refGenomeName, "Reference genome.", true);
    clp.RegisterStringOption("mutGenome", &mutGenomeName, "Mutated genome.", true);
    clp.RegisterPreviousFlagsAsHidden();
    clp.RegisterStringOption("gff", &gffFileName,
                             "GFF file describing the modifications made to the genome.");
    clp.RegisterFloatOption("i", &insRate, "Insertion rate: (0-1].",
                            CommandLineParser::NonNegativeFloat, false);
    clp.RegisterFloatOption("d", &delRate, "Deletion rate: (0-1]",
                            CommandLineParser::NonNegativeFloat, false);
    clp.RegisterFloatOption("m", &mutRate, "Mutation rate, even across all nucleotides: (0-1]",
                            CommandLineParser::NonNegativeFloat, false);
    clp.RegisterFlagOption("lower", &lower, "Make mutations in lower case", false);
    std::vector<std::string> leftovers;
    clp.ParseCommandLine(argc, argv, leftovers);

    FASTAReader reader;
    FASTASequence refGenome;

    reader.Init(refGenomeName);
    std::ofstream mutGenomeOut;
    CrucialOpen(mutGenomeName, mutGenomeOut, std::ios::out);
    std::ofstream gffOut;
    if (gffFileName != "") {
        CrucialOpen(gffFileName, gffOut, std::ios::out);
    }

    std::vector<int> insIndices, delIndices, subIndices;
    int readIndex = 0;
    InitializeRandomGeneratorWithTime();
    while (reader.GetNext(refGenome)) {
        insIndices.resize(refGenome.length);
        delIndices.resize(refGenome.length);
        subIndices.resize(refGenome.length);
        std::fill(insIndices.begin(), insIndices.end(), false);
        std::fill(delIndices.begin(), delIndices.end(), false);
        std::fill(subIndices.begin(), subIndices.end(), 0);

        enum ChangeType
        {
            Ins,
            Del,
            Mut,
            None
        };
        float changeProb[4];
        changeProb[Ins] = insRate;
        changeProb[Del] = changeProb[Ins] + delRate;
        changeProb[Mut] = changeProb[Del] + mutRate;
        changeProb[None] = 1;

        if (changeProb[Mut] > 1) {
            std::cout << "ERROR! The sum of the error probabilities must be less than 1"
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }
        DNALength pos;
        float randomNumber;
        int numIns = 0;
        int numDel = 0;
        int numMut = 0;
        for (pos = 0; pos < refGenome.length; pos++) {
            randomNumber = Random();
            if (randomNumber < changeProb[Ins]) {
                insIndices[pos] = true;
                numIns++;
            } else if (randomNumber < changeProb[Del]) {
                delIndices[pos] = true;
                numDel++;
            } else if (randomNumber < changeProb[Mut]) {
                Nucleotide newNuc = TwoBitToAscii[RandomInt(4)];
                int maxIts = 100000;
                int it = 0;
                while (newNuc == refGenome.seq[pos]) {
                    newNuc = TwoBitToAscii[RandomInt(4)];
                    if (it == maxIts) {
                        std::cout
                            << "ERROR, something is wrong with the random number generation, it "
                               "took too many tries to generate a new nucleotide"
                            << std::endl;
                        std::exit(EXIT_FAILURE);
                    }
                }
                subIndices[pos] = refGenome[pos];
                refGenome.seq[pos] = ToLower(newNuc, lower);
                ++numMut;
            }
        }
        //		std::cout << readIndex << " m " << numMut << " i " << numIns << " d " << numDel << std::endl;
        if (readIndex % 100000 == 0 && readIndex > 0) {
            std::cout << readIndex << std::endl;
        }
        //
        // Now add the insertions and deletions.
        //
        FASTASequence newSequence;
        DNALength newPos;
        if (numIns - numDel + static_cast<int>(refGenome.length) < 0) {
            std::cout << "ERROR, the genome has been deleted to nothing." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        ResizeSequence(newSequence, refGenome.length + (numIns - numDel));
        newPos = 0;
        pos = 0;
        for (pos = 0; pos < refGenome.length; pos++) {
            assert(newPos < newSequence.length or delIndices[pos] == true);
            if (subIndices[pos] != 0 and gffFileName != "") {
                gffOut << refGenome.GetName() << "	.	SNV	" << newPos << " " << newPos
                       << " 0.00	.	.	reference=" << (char)subIndices[pos]
                       << ";confidence=10;Name=" << newPos << (char)subIndices[pos] << ">"
                       << refGenome.seq[pos] << ";coverage=10;variantseq=" << refGenome.seq[pos]
                       << std::endl;
            }

            if (insIndices[pos] == true) {
                newSequence.seq[newPos] = ToLower(TwoBitToAscii[RandomInt(4)], lower);
                newPos++;
                newSequence.seq[newPos] = refGenome.seq[pos];

                assert(newSequence.seq[newPos] != '1');
                assert(newSequence.seq[newPos] != 1);
                if (gffFileName != "") {
                    gffOut << refGenome.GetName() << "	.	deletion	" << newPos << " " << newPos
                           << " 0.00	.	.	reference=" << newSequence.seq[newPos]
                           << ";length=1;confidence=10;coverage=0;Name=" << newPos << "del"
                           << newSequence.seq[newPos] << std::endl;
                }
                newPos++;
            } else if (delIndices[pos] == true) {
                // no-op, skip
                if (gffFileName != "") {
                    gffOut << refGenome.GetName() << "	.	insertion	" << newPos << " " << newPos
                           << " 0.00	.	.	confidence=10;Name=" << newPos << "_ins"
                           << refGenome.seq[pos] << ";reference=.;length=1;coverage=0;variantseq="
                           << refGenome.seq[newPos] << std::endl;
                    //ref000001	.	deletion	20223	20223	0.00	.	.	reference=T;length=1;confidence=0;coverage=0;Name=20222delT
                }
            } else {
                newSequence.seq[newPos] = refGenome.seq[pos];
                newPos++;
            }
        }
        std::stringstream titlestrm;
        titlestrm << " mutated ins " << insRate << " del " << delRate << " mut " << mutRate;
        newSequence.CopyTitle(refGenome.title);
        newSequence.AppendToTitle(titlestrm.str());
        newSequence.PrintSeq(mutGenomeOut);
        newSequence.Free();
        readIndex++;
    }
}
