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

#include <pbdata/defs.h>
#include <alignment/MappingMetrics.hpp>
#include <alignment/algorithms/alignment/AffineGuidedAlign.hpp>
#include <alignment/algorithms/alignment/AffineKBandAlign.hpp>
#include <alignment/algorithms/alignment/AlignmentFormats.hpp>
#include <alignment/algorithms/alignment/AlignmentUtils.hpp>
#include <alignment/algorithms/alignment/DistanceMatrixScoreFunction.hpp>
#include <alignment/algorithms/alignment/ExtendAlign.hpp>
#include <alignment/algorithms/alignment/FullQVAlign.hpp>
#include <alignment/algorithms/alignment/GuidedAlign.hpp>
#include <alignment/algorithms/alignment/IDSScoreFunction.hpp>
#include <alignment/algorithms/alignment/OneGapAlignment.hpp>
#include <alignment/algorithms/alignment/QualityValueScoreFunction.hpp>
#include <alignment/algorithms/alignment/StringToScoreMatrix.hpp>
#include <alignment/algorithms/anchoring/BWTSearch.hpp>
#include <alignment/algorithms/anchoring/ClusterProbability.hpp>
#include <alignment/algorithms/anchoring/FindMaxInterval.hpp>
#include <alignment/algorithms/anchoring/LISPValue.hpp>
#include <alignment/algorithms/anchoring/LISPValueWeightor.hpp>
#include <alignment/algorithms/anchoring/LISQValueWeightor.hpp>
#include <alignment/algorithms/anchoring/LISSizeWeightor.hpp>
#include <alignment/algorithms/anchoring/MapBySuffixArray.hpp>
#include <alignment/algorithms/compare/CompareStrings.hpp>
#include <alignment/bwt/BWT.hpp>
#include <alignment/datastructures/alignment/AlignmentCandidate.hpp>
#include <alignment/datastructures/alignment/AlignmentContext.hpp>
#include <alignment/datastructures/anchoring/AnchorParameters.hpp>
#include <alignment/datastructures/anchoring/ClusterList.hpp>
#include <alignment/datastructures/anchoring/WeightedInterval.hpp>
#include <alignment/files/CCSIterator.hpp>
#include <alignment/files/FragmentCCSIterator.hpp>
#include <alignment/files/ReaderAgglomerate.hpp>
#include <alignment/format/BAMPrinter.hpp>
#include <alignment/format/CompareSequencesPrinter.hpp>
#include <alignment/format/IntervalPrinter.hpp>
#include <alignment/format/SAMHeaderPrinter.hpp>
#include <alignment/format/SAMPrinter.hpp>
#include <alignment/format/StickAlignmentPrinter.hpp>
#include <alignment/format/SummaryPrinter.hpp>
#include <alignment/format/VulgarPrinter.hpp>
#include <alignment/format/XMLPrinter.hpp>
#include <alignment/statistics/LookupAnchorDistribution.hpp>
#include <alignment/statistics/StatUtils.hpp>
#include <alignment/statistics/VarianceAccumulator.hpp>
#include <alignment/statistics/cdfs.hpp>
#include <alignment/statistics/pdfs.hpp>
#include <alignment/suffixarray/SharedSuffixArray.hpp>
#include <alignment/suffixarray/SuffixArrayTypes.hpp>
#include <alignment/tuples/DNATuple.hpp>
#include <alignment/tuples/HashedTupleList.hpp>
#include <alignment/tuples/TupleCountTable.hpp>
#include <alignment/utils/FileOfFileNames.hpp>
#include <alignment/utils/RegionUtils.hpp>
#include <hdf/HDFRegionTableReader.hpp>
#include <pbdata/CCSSequence.hpp>
#include <pbdata/CommandLineParser.hpp>
#include <pbdata/FASTAReader.hpp>
#include <pbdata/FASTASequence.hpp>
#include <pbdata/PackedDNASequence.hpp>
#include <pbdata/SMRTSequence.hpp>
#include <pbdata/SeqUtils.hpp>
#include <pbdata/metagenome/SequenceIndexDatabase.hpp>
#include <pbdata/metagenome/TitleTable.hpp>
#include <pbdata/qvs/QualityTransform.hpp>
#include <pbdata/qvs/QualityValue.hpp>
#include <pbdata/reads/ReadInterval.hpp>
#include <pbdata/utils.hpp>
#include <pbdata/utils/SMRTTitle.hpp>
#include <pbdata/utils/TimeUtils.hpp>

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
