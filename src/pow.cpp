// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>
#include <util.h>
#include "bignum.h"

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, int algo, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    if (params.fPowAllowMinDifficultyBlocks)
    {
        // Special difficulty rule for testnet:
        // If the new block's timestamp is more than 2* 10 minutes
        // then allow mining of a min-difficulty block.
        // TODO Badcoin: enable this at the next hard fork for testnet
        /*
        if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
            return nProofOfWorkLimit;
        */
    }

    // find previous block with same algo
    const CBlockIndex* pindexPrev = GetLastBlockIndexForAlgo(pindexLast, algo);

    // Genesis block check again
    if (pindexPrev == NULL)
        return nProofOfWorkLimit;

    const CBlockIndex* pindexFirst = NULL;

    // find first block in averaging interval
    // Go back by what we want to be nAveragingInterval blocks
    pindexFirst = pindexPrev;
    for (int i = 0; pindexFirst && i < params.nAveragingInterval - 1; i++)
    {
        pindexFirst = pindexFirst->pprev;
        pindexFirst = GetLastBlockIndexForAlgo(pindexFirst, algo);
        if (pindexFirst == NULL) {
            LogPrint(BCLog::ALL,"pindexFirst is null. returning nProofOfWorkLimit\n");
            return nProofOfWorkLimit;
        }
    }

    int64_t nActualTimespan;
    nActualTimespan = pindexPrev->GetMedianTimePast() - pindexFirst->GetMedianTimePast();
    LogPrint(BCLog::ALL,"  nActualTimespan = %d before bounds   %d   %d\n", nActualTimespan, pindexPrev->GetMedianTimePast(), pindexFirst->GetMedianTimePast());
    return CalculateNextWorkRequired(pindexPrev, pindexFirst, params, algo, nActualTimespan, pindexLast->nHeight);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexPrev, const CBlockIndex* pindexFirst, const Consensus::Params& params, int algo, int64_t nActualTimespan, int nHeight)
{
    if (params.fPowNoRetargeting)
        return pindexPrev->nBits;

    const arith_uint256 nProofOfWorkLimit = UintToArith256(params.powLimit);    
    
    int64_t nTargetSpacingPerAlgo = params.nPowTargetSpacing * NUM_ALGOS; // 60 * 5 = 300s per algo
    int64_t nAveragingTargetTimespan = params.nAveragingInterval * nTargetSpacingPerAlgo; // 10 blocks per algo
    int64_t nMinActualTimespan = nAveragingTargetTimespan * (100 - params.nMaxAdjustUp) / 100;
    int64_t nMaxActualTimespan = nAveragingTargetTimespan * (100 + params.nMaxAdjustDown) / 100;
    
    if (nActualTimespan < nMinActualTimespan)
        nActualTimespan = nMinActualTimespan;
    if (nActualTimespan > nMaxActualTimespan)
        nActualTimespan = nMaxActualTimespan;
    
    LogPrint(BCLog::ALL,"  nActualTimespan = %d after bounds   %d   %d\n", nActualTimespan, nMinActualTimespan, nMaxActualTimespan);
    
    arith_uint256 bnNew;
    arith_uint256 bnOld;
    bnNew.SetCompact(pindexPrev->nBits);
    bnOld = bnNew;
    bnNew *= nActualTimespan;
    bnNew /= nAveragingTargetTimespan;
    if (bnNew > nProofOfWorkLimit)
        bnNew = nProofOfWorkLimit;
    
    /// debug print
    LogPrint(BCLog::ALL,"CalculateNextWorkRequired(Algo=%d): RETARGET\n", algo);
    LogPrint(BCLog::ALL,"CalculateNextWorkRequired(Algo=%d): nTargetTimespan = %d    nActualTimespan = %d\n", algo, nAveragingTargetTimespan, nActualTimespan);
    LogPrint(BCLog::ALL,"CalculateNextWorkRequired(Algo=%d): Before: %08x  %s\n", algo, pindexPrev->nBits, bnOld.ToString());
    LogPrint(BCLog::ALL,"CalculateNextWorkRequired(Algo=%d): After:  %08x  %s\n", algo, bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, int algo, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
