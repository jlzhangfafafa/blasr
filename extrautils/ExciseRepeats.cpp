#include <sstream>
#include "FASTAReader.hpp"
#include "FASTASequence.hpp"
#include "utils.hpp"

int main(int argc, char* argv[])
{
    std::string seqInName, seqOutName, dotOutName;
    if (argc < 4) {
        std::cout << "usage: exciseRepeats inName repMaskOutFile outName" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    seqInName = argv[1];
    dotOutName = argv[2];
    seqOutName = argv[3];
    FASTAReader reader;
    reader.Initialize(seqInName);
    FASTASequence origSeq;
    reader.GetNext(origSeq);

    std::ifstream dotOutFile;
    CrucialOpen(dotOutName, dotOutFile);
    std::ofstream seqOutFile;
    std::ofstream seqOut;
    CrucialOpen(seqOutName, seqOut, std::ios::out);
    std::string dotOutLine;
    getline(dotOutFile, dotOutLine);
    getline(dotOutFile, dotOutLine);
    getline(dotOutFile, dotOutLine);
    while (getline(dotOutFile, dotOutLine)) {
        std::stringstream lineStrm(dotOutLine);
        int swScore;
        float pctDiv, pctDel, pctIns;
        std::string query;
        DNALength qPosBegin, qPosEnd;
        std::string left;
        char strand;
        std::string matchingRepeat;
        std::string repClass;
        std::string repPos, repEnd, repLeft;
        int id;
        lineStrm >> swScore >> pctDiv >> pctDel >> pctIns >> query >> qPosBegin >> qPosEnd >>
            left >> strand >> matchingRepeat >> repClass >> repPos >> repEnd >> repLeft >> id;
        for (DNALength seqPos = qPosBegin; seqPos < qPosEnd; seqPos++) {
            origSeq.seq[seqPos] = 'X';
        }
    }

    DNALength seqPos, unexPos;
    unexPos = 0;
    for (seqPos = 0; seqPos < origSeq.length; seqPos++) {
        if (origSeq.seq[seqPos] != 'X') {
            origSeq.seq[unexPos] = origSeq.seq[seqPos];
            unexPos++;
        }
    }
    origSeq.length = unexPos;

    origSeq.PrintSeq(seqOut);
    return 0;
}
