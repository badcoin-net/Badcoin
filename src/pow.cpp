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

    return KimotoGravityWell(pindexPrev, params, algo);
}

unsigned int KimotoGravityWell(const CBlockIndex* pindexLast, const Consensus::Params& params, int algo)
{
    const CBlockIndex  *BlockLastSolved = GetLastBlockIndexForAlgo(pindexLast, algo);
    const CBlockIndex  *BlockReading = GetLastBlockIndexForAlgo(pindexLast, algo);

    if (BlockLastSolved == NULL || BlockReading == NULL)
        return UintToArith256(params.powLimit).GetCompact();

    int64_t PastRateActualSeconds = 0;
    int64_t PastRateTargetSeconds = 0;
    double PastRateAdjustmentRatio = double(1);
    arith_uint256 PastDifficultyAverage;
    arith_uint256 PastDifficultyAveragePrev;
    double EventHorizonDeviation;
    double EventHorizonDeviationFast;
    double EventHorizonDeviationSlow;

    int64_t TargetBlocksSpacingSeconds = params.nPowTargetSpacing * NUM_ALGOS;
    int64_t PastSecondsMin = 60 * 60 * 2; // 2 Hours
    int64_t PastSecondsMax = 60 * 60 * 24 * 7; // A Week
    int64_t PastBlocksMin = PastSecondsMin / TargetBlocksSpacingSeconds;
    int64_t PastBlocksMax = PastSecondsMax / TargetBlocksSpacingSeconds; 

    LogPrint(BCLog::ALL,"Difficulty Retarget - Kimoto Gravity Well - algo %d\n", algo);

    for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) 
    {
        if (i > PastBlocksMax)
            break;

        if (i == 1)
            PastDifficultyAverage.SetCompact(BlockReading->nBits);
        else
            PastDifficultyAverage = ((arith_uint256().SetCompact(BlockReading->nBits) - PastDifficultyAveragePrev) / i) + PastDifficultyAveragePrev; 

        PastDifficultyAveragePrev = PastDifficultyAverage;
        PastRateActualSeconds = BlockLastSolved->GetBlockTime() - BlockReading->GetBlockTime();
        PastRateTargetSeconds = TargetBlocksSpacingSeconds * i;
        PastRateAdjustmentRatio = double(1);

        if (PastRateActualSeconds <= 0)
            PastRateActualSeconds = 0;

        if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0)
            PastRateAdjustmentRatio = double(PastRateTargetSeconds) / double(PastRateActualSeconds);

        EventHorizonDeviation = 1 + (0.7084 * pow((double(i)/double(144)), -1.228));
        EventHorizonDeviationFast = EventHorizonDeviation;
        EventHorizonDeviationSlow = 1 / EventHorizonDeviation;
        
        if (i >= PastBlocksMin) {
            if (PastRateAdjustmentRatio <= EventHorizonDeviationSlow) {
                LogPrint(BCLog::ALL,"Difficulty Retarget - (PastRateAdjustmentRatio <= EventHorizonDeviationSlow)\n");
                assert(BlockReading);
                break;
            }

            if (PastRateAdjustmentRatio >= EventHorizonDeviationFast) {
                LogPrint(BCLog::ALL,"Difficulty Retarget - (PastRateAdjustmentRatio >= EventHorizonDeviationFast)\n");
                assert(BlockReading);
                break;
            }
        }

        BlockReading = GetLastBlockIndexForAlgo(BlockReading->pprev, algo);
    }
    
    arith_uint256 bnNew(PastDifficultyAverage);
    if (PastRateTargetSeconds != 0) {
        bnNew *= PastRateActualSeconds;
        bnNew /= PastRateTargetSeconds;
    }
    if (bnNew > UintToArith256(params.powLimit)) { bnNew = UintToArith256(params.powLimit); }
    
    /// debug print
    LogPrint(BCLog::ALL,"PastRateActualSeconds = %g\n", PastRateActualSeconds);
    LogPrint(BCLog::ALL,"PastRateTargetSeconds = %g\n", PastRateTargetSeconds);
    LogPrint(BCLog::ALL,"PastRateAdjustmentRatio = %g\n", PastRateAdjustmentRatio);
    LogPrint(BCLog::ALL,"Before: %08x  %s\n", BlockLastSolved->nBits, arith_uint256().SetCompact(BlockLastSolved->nBits).ToString().c_str());
    LogPrint(BCLog::ALL,"After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString().c_str());
    
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
