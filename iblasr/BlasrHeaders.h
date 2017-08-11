#pragma once

#ifdef __GLIBC__
#include <mcheck.h>
#endif
#include <pthread.h>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#if defined(__GLIBC__) || defined(__APPLE__)
#include <execinfo.h>
#endif

#define MAX_PHRED_SCORE 254
#define MAPQV_END_ALIGN_WIGGLE 5

#include <LibBlasrConfig.h>
#ifdef USE_PBBAM
#include <pbbam/BamWriter.h>
#include <pbbam/SamWriter.h>
#endif

#include <defs.h>
#include <CCSSequence.hpp>
#include <FASTAReader.hpp>
#include <FASTASequence.hpp>
#include <SMRTSequence.hpp>
#include <SeqUtils.hpp>
#include <utils.hpp>

#include <CommandLineParser.hpp>
#include <HDFRegionTableReader.hpp>
#include <MappingMetrics.hpp>
#include <PackedDNASequence.hpp>
#include <algorithms/alignment/AffineGuidedAlign.hpp>
#include <algorithms/alignment/AffineKBandAlign.hpp>
#include <algorithms/alignment/AlignmentFormats.hpp>
#include <algorithms/alignment/AlignmentUtils.hpp>
#include <algorithms/alignment/DistanceMatrixScoreFunction.hpp>
#include <algorithms/alignment/ExtendAlign.hpp>
#include <algorithms/alignment/FullQVAlign.hpp>
#include <algorithms/alignment/GuidedAlign.hpp>
#include <algorithms/alignment/IDSScoreFunction.hpp>
#include <algorithms/alignment/OneGapAlignment.hpp>
#include <algorithms/alignment/QualityValueScoreFunction.hpp>
#include <algorithms/alignment/StringToScoreMatrix.hpp>
#include <algorithms/anchoring/BWTSearch.hpp>
#include <algorithms/anchoring/ClusterProbability.hpp>
#include <algorithms/anchoring/FindMaxInterval.hpp>
#include <algorithms/anchoring/LISPValue.hpp>
#include <algorithms/anchoring/LISPValueWeightor.hpp>
#include <algorithms/anchoring/LISQValueWeightor.hpp>
#include <algorithms/anchoring/LISSizeWeightor.hpp>
#include <algorithms/anchoring/MapBySuffixArray.hpp>
#include <algorithms/compare/CompareStrings.hpp>
#include <bwt/BWT.hpp>
#include <datastructures/alignment/AlignmentCandidate.hpp>
#include <datastructures/alignment/AlignmentContext.hpp>
#include <datastructures/anchoring/AnchorParameters.hpp>
#include <datastructures/anchoring/ClusterList.hpp>
#include <datastructures/anchoring/WeightedInterval.hpp>
#include <files/CCSIterator.hpp>
#include <files/FragmentCCSIterator.hpp>
#include <files/ReaderAgglomerate.hpp>
#include <format/BAMPrinter.hpp>
#include <format/CompareSequencesPrinter.hpp>
#include <format/IntervalPrinter.hpp>
#include <format/SAMHeaderPrinter.hpp>
#include <format/SAMPrinter.hpp>
#include <format/StickAlignmentPrinter.hpp>
#include <format/SummaryPrinter.hpp>
#include <format/VulgarPrinter.hpp>
#include <format/XMLPrinter.hpp>
#include <metagenome/SequenceIndexDatabase.hpp>
#include <metagenome/TitleTable.hpp>
#include <qvs/QualityTransform.hpp>
#include <qvs/QualityValue.hpp>
#include <reads/ReadInterval.hpp>
#include <statistics/LookupAnchorDistribution.hpp>
#include <statistics/StatUtils.hpp>
#include <statistics/VarianceAccumulator.hpp>
#include <statistics/cdfs.hpp>
#include <statistics/pdfs.hpp>
#include <suffixarray/SharedSuffixArray.hpp>
#include <suffixarray/SuffixArrayTypes.hpp>
#include <tuples/DNATuple.hpp>
#include <tuples/HashedTupleList.hpp>
#include <tuples/TupleCountTable.hpp>
#include <utils/FileOfFileNames.hpp>
#include <utils/RegionUtils.hpp>
#include <utils/SMRTTitle.hpp>
#include <utils/TimeUtils.hpp>

#include "MappingBuffers.hpp"
#include "MappingIPC.h"
#include "MappingSemaphores.h"
#include "ReadAlignments.hpp"

typedef SMRTSequence T_Sequence;
typedef FASTASequence T_GenomeSequence;
typedef DNASuffixArray T_SuffixArray;
typedef DNATuple T_Tuple;
typedef LISPValueWeightor<T_GenomeSequence, DNATuple, std::vector<ChainedMatchPos> > PValueWeightor;
typedef LISSMatchFrequencyPValueWeightor<T_GenomeSequence, DNATuple, std::vector<ChainedMatchPos> >
    MultiplicityPValueWeightor;
typedef MappingData<T_SuffixArray, T_GenomeSequence, T_Tuple> MappingIPC;
