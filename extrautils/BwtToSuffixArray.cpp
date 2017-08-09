#include "bwt/BWT.hpp"
#include "suffixarray/SuffixArray.hpp"
#include "suffixarray/SuffixArrayTypes.hpp"

#include <cstring>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{

    std::string bwtFileName, saFileName;
    if (argc < 3) {
        std::cout << "usage: bwt2sa bwtfile safile " << std::endl;
        exit(1);
    }
    bwtFileName = argv[1];
    saFileName = argv[2];

    Bwt<PackedDNASequence, FASTASequence> bwt;
    DNASuffixArray suffixArray;

    bwt.Read(bwtFileName);
    suffixArray.AllocateSuffixArray(bwt.bwtSequence.length - 1);
    SAIndex index;
    for (index = 1; index < bwt.bwtSequence.length + 1; index++) {
        suffixArray.index[index - 1] = bwt.Locate(index);
    }
    suffixArray.Write(saFileName);
}
