#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

#include <datastructures/alignment/Path.h>
#include <defs.h>
#include <FASTAReader.hpp>
#include <FASTASequence.hpp>
#include <algorithms/alignment/AlignmentUtils.hpp>
#include <algorithms/alignment/SDPAlign.hpp>
#include <algorithms/alignment/SWAlign.hpp>
#include <datastructures/alignment/Alignment.hpp>
#include <format/StickAlignmentPrinter.hpp>
#include <tuples/DNATuple.hpp>
#include <tuples/TupleMetrics.hpp>

/*
 * Performs sparse dynamic programming (SDP) between pairs of sequences as they
 * are given in two FASTA files, one called for convenience query, the other
 * target. k is the size of the k-mer used for the SDP algorithm.
 */

void PrintUsage()
{
    std::cout << "usage: sdpMatcher query target k [-indelRate delta] "
                 "[-showalign] [-printsw] [-noRefine] [-indel i] [ -local ] "
                 "[-match m] [-sdpIndel i]"
              << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 4) {
        PrintUsage();
        std::exit(EXIT_FAILURE);
    }

    std::string queryName, targetName;
    queryName = argv[1];
    targetName = argv[2];
    TupleMetrics tm;
    tm.Initialize(atoi(argv[3]));
    int argi = 4;
    float indelRate = 0.25;
    int indel = 3;
    int match = 0;
    int printSW = 0;
    int printSimilarity = 0;
    int refineAlignments = 1;
    int showalign = 0;
    int fixedTarget = 0;
    int sdpIndel = indel;
    int sdpIns = 5;
    int sdpDel = 5;
    (void)(sdpIndel);
    (void)(sdpIns);
    (void)(sdpDel);  // not yet used.
    AlignmentType alignType = Global;
    while (argi < argc) {
        if (strcmp(argv[argi], "-indelRate") == 0) {
            ++argi;
            indelRate = atof(argv[argi]);
        } else if (strcmp(argv[argi], "-printsw") == 0) {
            printSW = 1;
        } else if (strcmp(argv[argi], "-noRefine") == 0) {
            refineAlignments = 0;
        } else if (strcmp(argv[argi], "-indel") == 0) {
            indel = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-sdpIndel") == 0) {
            sdpIndel = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-sdpIns") == 0) {
            sdpIns = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-sdpDel") == 0) {
            sdpDel = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-showalign") == 0) {
            showalign = 1;
        } else if (strcmp(argv[argi], "-local") == 0) {
            alignType = Local;
        } else if (strcmp(argv[argi], "-match") == 0) {
            match = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-fixedtarget") == 0) {
            fixedTarget = 1;
        } else if (strcmp(argv[argi], "-printSimilarity") == 0) {
            printSimilarity = 1;
        } else {
            PrintUsage();
            std::cout << "Bad option: " << argv[argi] << std::endl;
            std::exit(EXIT_FAILURE);
        }
        ++argi;
    }

    FASTASequence query, target;
    FASTAReader queryReader, targetReader;
    queryReader.Init(queryName);

    targetReader.Init(targetName);

    if (match != 0) {
        int i;
        for (i = 0; i < 4; i++) {
            LocalAlignLowMutationMatrix[i][i] = match;
        }
    }

    int seqIndex = 0;
    Alignment alignment;
    std::vector<int> scoreMat;
    std::vector<Arrow> pathMat;
    DistanceMatrixScoreFunction<DNASequence, DNASequence> distScoreFn;
    distScoreFn.del = indel;
    distScoreFn.ins = indel;
    distScoreFn.InitializeScoreMatrix(SMRTDistanceMatrix);

    if (fixedTarget) {
        targetReader.GetNext(target);
    }

    std::cout << "qid,tid,qstart,qend,qlen,tstart,tend,tlen,score";
    if (printSimilarity) std::cout << ",pctSimilarity";
    std::cout << std::endl;

    while (queryReader.GetNext(query) and (fixedTarget or targetReader.GetNext(target))) {

        if (query.length == 0 or target.length == 0) continue;
        alignment.blocks.clear();

        int alignScore;
        alignScore = SDPAlign(query, target, distScoreFn, tm.tupleSize, sdpIndel, sdpIndel,
                              indelRate, alignment, alignType, refineAlignments, false, 0);

        ComputeAlignmentStats(alignment, query.seq, target.seq, distScoreFn);

        if (alignScore > 0) {  // in rare cases the SDP returns positive.
            alignScore = 0;    // this makes it more like a true local alignment
        }

        if (showalign) {
            StickPrintAlignment(alignment, query, target, std::cout);
        }

        if (printSW) {
            MatchedAlignment swAlignment;
            std::vector<int> scoreMat;
            std::vector<Arrow> pathMat;
            SWAlign(query, target, scoreMat, pathMat, swAlignment, distScoreFn);
            StickPrintAlignment(swAlignment, query, target, std::cout);
        }

        std::cout << query.GetName() << "," << target.GetName() << "," << alignment.qPos << ","
                  << alignment.QEnd() << "," << query.length << "," << alignment.tPos << ","
                  << alignment.TEnd() << "," << target.length << "," << alignScore;
        if (printSimilarity) std::cout << "," << alignment.pctSimilarity;
        std::cout << std::endl;

        ++seqIndex;
    }

    return 0;
}
