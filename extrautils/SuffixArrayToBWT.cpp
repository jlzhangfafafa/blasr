#include <fstream>
#include <iostream>

#include <alignment/bwt/BWT.hpp>
#include <alignment/suffixarray/SuffixArray.hpp>
#include <alignment/suffixarray/SuffixArrayTypes.hpp>
#include <pbdata/FASTAReader.hpp>
#include <pbdata/FASTASequence.hpp>

int main(int argc, char* argv[])
{
    if (argc < 4) {
        std::cout << "usage: sa2bwt genomeFileName suffixArray bwt [-debug]" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::string genomeFileName = argv[1];
    std::string suffixArrayFileName = argv[2];
    std::string bwtFileName = argv[3];
    int storeDebugInformation = 0;
    int argi = 4;
    while (argi < argc) {
        if (strcmp(argv[argi], "-debug") == 0) {
            storeDebugInformation = 1;
        }
        ++argi;
    }

    std::ofstream bwtOutFile;
    CrucialOpen(bwtFileName, bwtOutFile, std::ios::out | std::ios::binary);

    FASTAReader reader;
    reader.Init(genomeFileName);
    FASTASequence seq;
    reader.ReadAllSequencesIntoOne(seq);

    DNASuffixArray suffixArray;
    suffixArray.Read(suffixArrayFileName);

    Bwt<PackedDNASequence, FASTASequence> bwt;
    bwt.InitializeFromSuffixArray(seq, suffixArray.index, storeDebugInformation);
    bwt.Write(bwtOutFile);

    return 0;
}
