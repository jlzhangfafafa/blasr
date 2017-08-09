// Author: Mark Chaisson
#pragma once

#include "BlasrHeaders.h"

//----------------------MODIFY ALIGNMENTS--------------------------//
//FIXME: refactor class SequenceIndexDatabase
void AssignRefContigLocation(T_AlignmentCandidate &alignment,
                             SequenceIndexDatabase<FASTQSequence> &seqdb, DNASequence &genome);

//FIXME: refactor class SequenceIndexDatabase
void AssignRefContigLocations(std::vector<T_AlignmentCandidate *> &alignmentPtrs,
                              SequenceIndexDatabase<FASTQSequence> &seqdb, DNASequence &genome);

template <typename T_RefSequence>
//FIXME: refactor class SequenceIndexDatabase
void AssignGenericRefContigName(std::vector<T_AlignmentCandidate *> &alignmentPtrs,
                                T_RefSequence &genome);

//FIXME: move to class ReadAlignments
void StoreRankingStats(std::vector<T_AlignmentCandidate *> &alignments,
                       VarianceAccumulator<float> &accumPValue,
                       VarianceAccumulator<float> &accumWeight);

//FIXME: mapQV should be assigned when alignments are created.
void AssignMapQV(std::vector<T_AlignmentCandidate *> &alignmentPtrs);

//FIXME: move to class ReadAlignments
void ScaleMapQVByClusterSize(T_AlignmentCandidate &alignment, MappingParameters &params);

void StoreMapQVs(SMRTSequence &read, std::vector<T_AlignmentCandidate *> &alignmentPtrs,
                 MappingParameters &params);

//--------------------SEARCH & CHECK ALIGNMENTS-------------------//
//FIXME: move to class ReadAlignments
template <typename T_Sequence>
bool CheckForSufficientMatch(T_Sequence &read, std::vector<T_AlignmentCandidate *> &alignmentPtrs,
                             MappingParameters &params);

//FIXME: move to class ReadAlignments
int FindMaxLengthAlignment(std::vector<T_AlignmentCandidate *> alignmentPtrs, int &maxLengthIndex);

//FIXME: move to class T_AlignmentCandidate
void SumMismatches(SMRTSequence &read, T_AlignmentCandidate &alignment, int mismatchScore,
                   int fullIntvStart, int fullIntvEnd, MappingParameters &params, int &sum);

//FIXME: move to class T_AlignmentCandidate
/// \returns whether two alignments overlap by more than minPcercentOverlap%
bool AlignmentsOverlap(T_AlignmentCandidate &alnA, T_AlignmentCandidate &alnB,
                       float minPercentOverlap);

/// \Partition overlapping alignments.
void PartitionOverlappingAlignments(std::vector<T_AlignmentCandidate *> &alignmentPtrs,
                                    std::vector<std::set<int> > &partitions, float minOverlap);

//--------------------FILTER ALIGNMENTS---------------------------//
//FIXME: move to class T_AlignmentCandidate and ReadAlignments
int RemoveLowQualitySDPAlignments(int readLength,
                                  std::vector<T_AlignmentCandidate *> &alignmentPtrs,
                                  MappingParameters &params);

//FIXME: move to class ReadAlignments
template <typename T_Sequence>
int RemoveLowQualityAlignments(T_Sequence &read, std::vector<T_AlignmentCandidate *> &alignmentPtrs,
                               MappingParameters &params);

//FIXME: move to class ReadAlignments
int RemoveOverlappingAlignments(std::vector<T_AlignmentCandidate *> &alignmentPtrs,
                                MappingParameters &params);

// FIXME: move to class ReadAlignments
// Delete all alignments from index startIndex in vector, inclusive.
void DeleteAlignments(std::vector<T_AlignmentCandidate *> &alignmentPtrs, int startIndex = 0);

//--------------------REFINE ALIGNMENTS---------------------------//
template <typename T_RefSequence, typename T_Sequence>
void RefineAlignment(std::vector<T_Sequence *> &bothQueryStrands, T_RefSequence &genome,
                     T_AlignmentCandidate &alignmentCandidate, MappingParameters &params,
                     MappingBuffers &mappingBuffers);

template <typename T_RefSequence, typename T_Sequence>
void RefineAlignments(std::vector<T_Sequence *> &bothQueryStrands, T_RefSequence &genome,
                      std::vector<T_AlignmentCandidate *> &alignmentPtrs, MappingParameters &params,
                      MappingBuffers &mappingBuffers);

//--------------------PRINT ALIGNMENTS---------------------------//
std::vector<T_AlignmentCandidate *> SelectAlignmentsToPrint(
    std::vector<T_AlignmentCandidate *> alignmentPtrs, MappingParameters &params,
    const int &associatedRandInt);

//
// The full read is not the subread, and does not have masked off characters.
//
void PrintAlignment(T_AlignmentCandidate &alignment, SMRTSequence &fullRead,
                    MappingParameters &params, AlignmentContext &alignmentContext,
                    std::ostream &outFile
#ifdef USE_PBBAM
                    ,
                    SMRTSequence &subread, PacBio::BAM::IRecordWriter *bamWriterPtr
#endif
                    );

// Print all alignments in std::vector<T_AlignmentCandidate*> alignmentPtrs
void PrintAlignments(std::vector<T_AlignmentCandidate *> alignmentPtrs, SMRTSequence &read,
                     MappingParameters &params, std::ostream &outFile,
                     AlignmentContext alignmentContext,
#ifdef USE_PBBAM
                     SMRTSequence &subread, PacBio::BAM::IRecordWriter *bamWriterPtr,
#endif
                     MappingSemaphores &semaphores);

void PrintAlignmentPtrs(std::vector<T_AlignmentCandidate *> &alignmentPtrs,
                        std::ostream &out = std::cout);

// Print an unaligned read, if noPrintUnalignedSeqs is True, print title only;
// otherwise, print title and sequence of the read.
void PrintUnaligned(const SMRTSequence &unalignedRead, std::ostream &unalignedFilePtr,
                    const bool noPrintUnalignedSeqs);

// Print all alignments for subreads in allReadAlignments.
// Input:
//   allReadAlignments - contains a set of subreads, each of which
//                       is associated with a group of alignments.
//   alignmentContext  - an alignment context of each subread used
//                       for printing in SAM format.
//   params            - mapping parameters.
// Output:
//   outFilePtr        - where to print alignments for subreads.
//   unalignedFilePtr  - where to print sequences for unaligned subreads.
void PrintAllReadAlignments(ReadAlignments &allReadAlignments, AlignmentContext &alignmentContext,
                            std::ostream &outFilePtr, std::ostream &unalignedFilePtr,
                            MappingParameters &params, std::vector<SMRTSequence> &subreads,
#ifdef USE_PBBAM
                            PacBio::BAM::IRecordWriter *bamWriterPtr,
#endif
                            MappingSemaphores &semaphores);

#include "BlasrUtilsImpl.hpp"
