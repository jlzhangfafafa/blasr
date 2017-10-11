// Author: Mark Chaisson
#pragma once

#include <algorithms/alignment/sdp/SDPFragment.hpp>
#include <algorithms/anchoring/BasicEndpoint.hpp>
#include <datastructures/anchoring/ClusterList.hpp>
#include <datastructures/anchoring/MatchPos.hpp>
#include <tuples/DNATuple.hpp>
#include <tuples/TupleList.hpp>
#include <vector>

//
// Define a list of buffers that are meant to grow to high-water
// marks, and not shrink down past that.   The memory is reused rather
// than having multiple calls to new.
//
class MappingBuffers
{
public:
    std::vector<int> hpInsScoreMat, insScoreMat;
    std::vector<int> kbandScoreMat;
    std::vector<Arrow> hpInsPathMat, insPathMat;
    std::vector<Arrow> kbandPathMat;
    std::vector<int> scoreMat;
    std::vector<Arrow> pathMat;
    std::vector<int> affineScoreMat;
    std::vector<Arrow> affinePathMat;
    std::vector<ChainedMatchPos> matchPosList;
    std::vector<ChainedMatchPos> rcMatchPosList;
    std::vector<BasicEndpoint<ChainedMatchPos> > globalChainEndpointBuffer;
    std::vector<Fragment> sdpFragmentSet, sdpPrefixFragmentSet, sdpSuffixFragmentSet;
    TupleList<PositionDNATuple> sdpCachedTargetTupleList;
    TupleList<PositionDNATuple> sdpCachedTargetPrefixTupleList;
    TupleList<PositionDNATuple> sdpCachedTargetSuffixTupleList;
    std::vector<int> sdpCachedMaxFragmentChain;
    std::vector<double> probMat;
    std::vector<double> optPathProbMat;
    std::vector<float> lnSubPValueMat;
    std::vector<float> lnInsPValueMat;
    std::vector<float> lnDelPValueMat;
    std::vector<float> lnMatchPValueMat;
    std::vector<int> clusterNumBases;
    ClusterList clusterList;
    ClusterList revStrandClusterList;

    void Reset(void);
};

inline void MappingBuffers::Reset(void)
{
    std::vector<int>().swap(hpInsScoreMat);
    std::vector<int>().swap(insScoreMat);
    std::vector<int>().swap(kbandScoreMat);
    std::vector<Arrow>().swap(hpInsPathMat);
    std::vector<Arrow>().swap(insPathMat);
    std::vector<Arrow>().swap(kbandPathMat);
    std::vector<int>().swap(scoreMat);
    std::vector<Arrow>().swap(pathMat);
    std::vector<ChainedMatchPos>().swap(matchPosList);
    std::vector<ChainedMatchPos>().swap(rcMatchPosList);
    std::vector<BasicEndpoint<ChainedMatchPos> >().swap(globalChainEndpointBuffer);
    std::vector<Fragment>().swap(sdpFragmentSet);
    std::vector<Fragment>().swap(sdpPrefixFragmentSet);
    std::vector<Fragment>().swap(sdpSuffixFragmentSet);
    sdpCachedTargetTupleList.Reset();
    sdpCachedTargetPrefixTupleList.Reset();
    sdpCachedTargetSuffixTupleList.Reset();
    std::vector<int>().swap(sdpCachedMaxFragmentChain);
    std::vector<double>().swap(probMat);
    std::vector<double>().swap(optPathProbMat);
    std::vector<float>().swap(lnSubPValueMat);
    std::vector<float>().swap(lnInsPValueMat);
    std::vector<float>().swap(lnDelPValueMat);
    std::vector<float>().swap(lnMatchPValueMat);
    std::vector<int>().swap(clusterNumBases);
}
