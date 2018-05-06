#include <iostream>
#include <string>

#include <alignment/suffixarray/SuffixArray.hpp>
#include <alignment/suffixarray/SuffixArrayTypes.hpp>
#include <pbdata/utils.hpp>

int main(int argc, char* argv[])
{

    if (argc <= 1) {
        std::cout << "sals checks if a suffix array has lookup table or not." << std::endl;
        std::cout << "usage sals genome.sa" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string saFileName = argv[1];

    DNASuffixArray sa;
    if (!sa.LightRead(saFileName)) {
        std::cout << "The file is not in a sa format." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (sa.componentList[DNASuffixArray::CompArray]) {
        std::cout << " * has a suffix array." << std::endl;
    } else {
        std::cout << " * does not contain a suffix array." << std::endl;
    }

    if (sa.componentList[DNASuffixArray::CompLookupTable]) {
        std::cout << " * has a lookup table for word size. " << sa.lookupPrefixLength << std::endl;
    } else {
        std::cout << " * does not have a lookup table." << std::endl;
    }
}
