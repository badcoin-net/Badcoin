// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include "uint256.h"
#include <map>
#include <string>

namespace Consensus {

enum DeploymentPos
{
    DEPLOYMENT_TESTDUMMY,
    DEPLOYMENT_CSV, // Deployment of BIP68, BIP112, and BIP113.
    DEPLOYMENT_SEGWIT, // Deployment of BIP141, BIP143, and BIP147.
    DEPLOYMENT_LEGBIT, // Deployment of Legacy Bits.
    DEPLOYMENT_RESERVEALGO, // Deployment of MIP2 (Reserve algos)
    // NOTE: Also add new deployments to VersionBitsDeploymentInfo in versionbits.cpp
    MAX_VERSION_BITS_DEPLOYMENTS
};

/**
 * Struct for each individual consensus rule change using BIP9.
 */
struct BIP9Deployment {
    /** Bit position to select the particular bit in nVersion. */
    int bit;
    /** Start MedianTime for version bits miner confirmation. Can be a date in the past */
    int64_t nStartTime;
    /** Timeout/expiry MedianTime for the deployment attempt. */
    int64_t nTimeout;
};

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;
    int nSubsidyHalvingInterval;
    /** Block height and hash at which BIP34 becomes active */
    int BIP34Height;
    uint256 BIP34Hash;
    /** Block height at which BIP65 becomes active */
    int BIP65Height;
    /** Block height at which BIP66 becomes active */
    int BIP66Height;
    /** Block height at which MIP2 becomes active */
    int MIP2Height;
    /**
     * Minimum blocks including miner confirmation of the total of 2016 blocks in a retargeting period,
     * (nPowTargetTimespan / nPowTargetSpacing) which is also used for BIP9 deployments.
     * Examples: 1916 for 95%, 1512 for testchains.
     */
    uint32_t nRuleChangeActivationThreshold;
    uint32_t nMinerConfirmationWindow;
    BIP9Deployment vDeployments[MAX_VERSION_BITS_DEPLOYMENTS];
    /** Proof of work parameters */
    uint256 powLimit;
    bool fPowAllowMinDifficultyBlocks;
    int64_t nPowTargetSpacingV1;
    int64_t nPowTargetSpacingV2;
    bool fPowNoRetargeting;
    int64_t nPowTargetSpacing;
    int64_t nPowTargetTimespan;
    int64_t DifficultyAdjustmentInterval() const { return nPowTargetTimespan / nPowTargetSpacingV1; }
    uint256 nMinimumChainWork;
    uint256 defaultAssumeValid;
    int64_t nAveragingInterval;
    int64_t nBlockTimeWarpPreventStart1;
    int64_t nBlockTimeWarpPreventStart2;
    int64_t nBlockTimeWarpPreventStart3;
    int64_t Phase2Timespan_Start;
    int64_t nBlockDiffAdjustV2;
    int64_t nMaxAdjustDown;
    int64_t nMaxAdjustUpV1;
    int64_t nMaxAdjustUpV2;
    int64_t nBlockSequentialAlgoRuleStart1;
    int64_t nBlockSequentialAlgoRuleStart2;
    int nBlockSequentialAlgoMaxCount1;
    int nBlockSequentialAlgoMaxCount2;
    int nBlockSequentialAlgoMaxCount3;
    int64_t nBlockAlgoWorkWeightStart;
    int64_t nBlockAlgoNormalisedWorkStart;
    int64_t nBlockAlgoNormalisedWorkDecayStart1;
    int64_t nBlockAlgoNormalisedWorkDecayStart2;
    int64_t nGeoAvgWork_Start;
    int64_t nFork1MinBlock;
    
    /** Auxpow parameters */
    int16_t nAuxpowChainId;
    bool fStrictChainId;
    int nStartAuxPow;
};
} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H