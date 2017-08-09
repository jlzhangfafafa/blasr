#include <string>
#include <vector>
#include "FASTAReader.hpp"
#include "FASTASequence.hpp"
#include "NucConversion.hpp"
#include "algorithms/sorting/qsufsort.hpp"
#include "suffixarray/SuffixArray.hpp"
#include "suffixarray/SuffixArrayTypes.hpp"
#include "suffixarray/ssort.hpp"

void PrintUsage()
{
    std::cout << "samodify changes word size of input suffix array." << std::endl;
    std::cout << "Usage: samodify in.sa genome.fasta out.sa [-blt p]" << std::endl;
    std::cout << "       -blt p  Build a lookup table on prefixes of length 'p' " << std::endl;
}

int main(int argc, char* argv[])
{

    if (argc < 4) {
        PrintUsage();
        exit(1);
    }
    int argi = 1;
    std::string saInFile = argv[argi++];
    std::string genomeFileName = argv[argi++];
    std::string saOutFile = argv[argi++];
    std::vector<std::string> inFiles;

    int doBLT = 0;
    int doBLCP = 0;
    int bltPrefixLength = 0;
    int lcpLength = 0;
    int parsingOptions = 0;

    while (argi < argc) {
        if (strcmp(argv[argi], "-blt") == 0) {
            doBLT = 1;
            bltPrefixLength = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-blcp") == 0) {
            doBLCP = 1;
            lcpLength = atoi(argv[++argi]);
        } else {
            PrintUsage();
            std::cout << "Bad option: " << argv[argi] << std::endl;
            exit(1);
        }
        ++argi;
    }

    //
    // Read the suffix array to modify.
    //

    DNASuffixArray sa;
    sa.Read(saInFile);

    FASTAReader reader;
    reader.Initialize(genomeFileName);
    FASTASequence seq;
    reader.ReadAllSequencesIntoOne(seq);

    if (doBLT) {
        sa.BuildLookupTable(seq.seq, seq.length, bltPrefixLength);
    }

    if (doBLCP) {
        std::cout << "LCP Table not yet implemented." << std::endl;
    }

    sa.Write(saOutFile);
}
