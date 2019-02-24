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

unsigned int static DarkGravityWave(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params, int algo) {
    /* current difficulty formula, dash - DarkGravity v3, written by Evan Duffield - evan@dash.org */
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    int64_t nPastBlocks = 24;

    // make sure we have at least (nPastBlocks + 1) blocks, otherwise just return powLimit
    if (pblock == nullptr || pindexLast == nullptr || pindexLast->nHeight < nPastBlocks)
        return bnPowLimit.GetCompact();

    if (params.fPowAllowMinDifficultyBlocks) {
        // recent block is more than 2 hours old
        if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + (2 * 60 * 60)) {
            return bnPowLimit.GetCompact();
        }
        // recent block is more than 10 minutes old
        if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing * 4) {
            arith_uint256 bnNew = arith_uint256().SetCompact(pindexLast->nBits) * 10;
            if (bnNew > bnPowLimit) {
                bnNew = bnPowLimit;
            }
            return bnNew.GetCompact();
        }
    }

    const CBlockIndex *pindex = pindexLast;
    arith_uint256 bnPastTargetAvg;

    for (unsigned int nCountBlocks = 1; pindex && nCountBlocks <= nPastBlocks; nCountBlocks++) {
        arith_uint256 bnTarget = arith_uint256().SetCompact(pindex->nBits);
        if (nCountBlocks == 1) {
            bnPastTargetAvg = bnTarget;
        } else {
            // NOTE: that's not an average really...
            bnPastTargetAvg = (bnPastTargetAvg * nCountBlocks + bnTarget) / (nCountBlocks + 1);
        }

        if(pindex->pprev == nullptr)
            break;
        pindex = GetLastBlockIndexForAlgo(pindex->pprev, algo);
    }

    arith_uint256 bnNew(bnPastTargetAvg);

    if(pindex == nullptr || pindexLast == nullptr)
        return bnPowLimit.GetCompact();

    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindex->GetBlockTime();
    // NOTE: is this accurate? nActualTimespan counts it for (nPastBlocks - 1) blocks only...
    int64_t nTargetTimespan = nPastBlocks * params.nPowTargetSpacing * NUM_ALGOS;

    if (nActualTimespan < nTargetTimespan/3)
        nActualTimespan = nTargetTimespan/3;
    if (nActualTimespan > nTargetTimespan*3)
        nActualTimespan = nTargetTimespan*3;

    // Retarget
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    if (bnNew > bnPowLimit) {
        bnNew = bnPowLimit;
    }

    return bnNew.GetCompact();
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, int algo, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    if (params.fPowAllowMinDifficultyBlocks)
    {
        // Special difficulty rule for testnet:
        // If the new block's timestamp is more than 2* 10 minutes
        // then allow mining of a min-difficulty block.
        if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
            return nProofOfWorkLimit;
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

    return DarkGravityWave(pindexPrev, pblock, params, algo);
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
