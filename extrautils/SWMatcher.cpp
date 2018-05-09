#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include <alignment/algorithms/alignment/AlignmentUtils.hpp>
#include <alignment/algorithms/alignment/DistanceMatrixScoreFunction.hpp>
#include <alignment/algorithms/alignment/IDSScoreFunction.hpp>
#include <alignment/algorithms/alignment/SWAlign.hpp>
#include <alignment/format/StickAlignmentPrinter.hpp>
#include <pbdata/FASTAReader.hpp>
#include <pbdata/FASTASequence.hpp>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "usage: swMatcher query target [-indel i] [-local] [-showalign] " << std::endl
                  << "       [-type queryfit|overlap|global] [-match m ] [-mismatch m]" << std::endl
                  << "    or [-local] [-queryfit] [-overlap] [-fixedtarget] [-fixedquery]"
                  << std::endl
                  << "       [-printmatrix]" << std::endl
                  << "   Unless -showalign is specified, output is tabular and in the formt:"
                  << std::endl
                  << "   query_length target_length align_score query_start query_end target_start "
                     "target_end"
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string queryName, targetName;
    queryName = argv[1];
    targetName = argv[2];
    int argi = 3;
    int indelCost = 3;
    int showAlign = 0;
    AlignmentType alignType = Global;
    int match = 0;
    int mismatch = 0;
    int fixedTarget = 0;
    int fixedQuery = 0;
    bool printMatrix = false;
    int insertion = 4;
    int deletion = 5;
    while (argi < argc) {
        if (strcmp(argv[argi], "-insertion") == 0) {
            insertion = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-deletion") == 0) {
            deletion = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-local") == 0) {
            alignType = Local;
        } else if (strcmp(argv[argi], "-showalign") == 0) {
            showAlign = 1;
        } else if (strcmp(argv[argi], "-fixedtarget") == 0) {
            fixedTarget = 1;
        } else if (strcmp(argv[argi], "-fixedquery") == 0) {
            fixedQuery = 1;
        } else if (strcmp(argv[argi], "-type") == 0) {
            ++argi;
            if (strcmp(argv[argi], "queryfit") == 0) {
                alignType = QueryFit;
            } else if (strcmp(argv[argi], "targetfit") == 0) {
                alignType = TargetFit;
            } else if (strcmp(argv[argi], "overlap") == 0) {
                alignType = Overlap;
            } else if (strcmp(argv[argi], "global") == 0) {
                alignType = Global;
            } else if (strcmp(argv[argi], "tpqs") == 0) {
                alignType = TPrefixQSuffix;
            } else if (strcmp(argv[argi], "tsqp") == 0) {
                alignType = TSuffixQPrefix;
            } else {
                std::cout << " ERROR, aligntype must be one of queryfit, overlap, or global"
                          << std::endl;
                std::exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[argi], "-printmatrix") == 0) {
            printMatrix = true;
        } else if (strcmp(argv[argi], "-local") == 0) {
            alignType = Local;
        } else if (strcmp(argv[argi], "-queryfit") == 0) {
            alignType = QueryFit;
        } else if (strcmp(argv[argi], "-overlap") == 0) {
            alignType = Overlap;
        } else if (strcmp(argv[argi], "-match") == 0) {
            match = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-mismatch") == 0) {
            mismatch = atoi(argv[++argi]);
        }
        ++argi;
    }
    DistanceMatrixScoreFunction<FASTASequence, FASTASequence> scoreFn(SMRTDistanceMatrix, insertion,
                                                                      deletion);

    FASTASequence query, target;
    FASTAReader queryReader, targetReader;
    queryReader.Init(queryName);

    targetReader.Init(targetName);

    if (fixedTarget) {
        targetReader.GetNext(target);
    }
    if (fixedQuery) {
        queryReader.GetNext(query);
    }
    //
    // Prepare the target database;
    //

    //
    // Prepare the query match set.
    //

    int seqIndex = 0;

    std::vector<int> scoreMat;
    std::vector<Arrow> pathMat;
    int alignScore;
    MatchedAlignment alignment;

    if (match != 0) {
        int i;
        for (i = 0; i < 4; i++) {
            LocalAlignLowMutationMatrix[i][i] = match;
        }
    }

    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            if (i == j) continue;
            SMRTDistanceMatrix[i][j] += 3;
        }
    }

    std::cout << "qlen tlen score" << std::endl;
    while ((fixedQuery or queryReader.GetNext(query)) and
           (fixedTarget or targetReader.GetNext(target))) {
        alignment.qName.assign(query.title, query.titleLength);
        alignment.tName.assign(target.title, target.titleLength);
        alignment.blocks.clear();
        alignment.qPos = 0;
        alignment.tPos = 0;
        alignment.qStart = 0;
        alignment.tStart = 0;
        if (query.length == 0 or target.length == 0) continue;

        alignScore = SWAlign(query, target, scoreMat, pathMat, alignment, scoreFn, alignType, false,
                             printMatrix);

        std::cout << query.length << " " << target.length << " " << alignScore << std::endl;
        std::cout << alignment.qPos << " " << alignment.QEnd() << " " << alignment.tPos << " "
                  << alignment.TEnd() << std::endl;

        if (showAlign) {
            ComputeAlignmentStats(alignment, query.seq, target.seq, scoreFn);
            //SMRTDistanceMatrix, indelCost, indelCost);
            PrintAlignmentStats(alignment, std::cout);
            StickPrintAlignment(alignment, query, target, std::cout);
        }
        ++seqIndex;
    }

    return 0;
}
