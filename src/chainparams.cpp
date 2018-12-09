// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <consensus/merkle.h>
#include <primitives/pureheader.h>

#include <tinyformat.h>
#include <util.h>
#include <utilstrencodings.h>

#include <arith_uint256.h>

#include <assert.h>
#include <memory>

#include <chainparamsseeds.h>

static CBlock CreateGenesisBlock(const CScript& genesisInputScript, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = genesisInputScript;
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, const char* pszTimestamp="On 11/28/2018 the Badcoin Genesis was created")
{
    const CScript genesisInputScript = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(genesisInputScript, genesisOutputScript, nTime, nNonce, nBits, 1, 0);
}

void CChainParams::UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";

        /*** Badcoin Additional Chainparams ***/

        consensus.nPowTargetSpacingV1 = 30; // target time for block spacing across all algorithms
        consensus.nPowTargetSpacingV2 = 60; // new target time for block spacing across all algorithms
        consensus.nAveragingInterval = 10; // number of blocks to take the timespan of

        consensus.nStartAuxPow = 1; // Allow AuxPow blocks from this height
        consensus.nAuxpowChainId = 0x006A;
        consensus.fStrictChainId = false;

        consensus.nBlockTimeWarpPreventStart1 = 740500; // block where time warp 1 prevention starts
        consensus.nBlockTimeWarpPreventStart2 = 766000; // block where time warp 2 prevention starts
        consensus.nBlockTimeWarpPreventStart3 = 1048320; // block where time warp 3 prevention starts
        consensus.Phase2Timespan_Start = 1401000; // block where 60 second target time kicks in
        consensus.nBlockDiffAdjustV2 = 766000; // block where difficulty adjust V2 starts

        consensus.nMaxAdjustDown = 4; // 4% adjustment down
        consensus.nMaxAdjustUpV1 = 2; // 2% adjustment up
        consensus.nMaxAdjustUpV2 = 4; // 4% adjustment up     

        consensus.nBlockSequentialAlgoRuleStart1 = 740000; // block where sequential algo rule starts
        consensus.nBlockSequentialAlgoRuleStart2 = 766000; // block where sequential algo rule starts
        consensus.nBlockSequentialAlgoMaxCount1 = 6; // maximum sequential blocks of same algo
        consensus.nBlockSequentialAlgoMaxCount2 = 3; // maximum sequential blocks of same algo
        consensus.nBlockSequentialAlgoMaxCount3 = 6; // maximum sequential blocks of same algo

        consensus.nBlockAlgoWorkWeightStart = 142000; // block where algo work weighting starts
        consensus.nBlockAlgoNormalisedWorkStart = 740000; // block where algo combined weight starts
        consensus.nBlockAlgoNormalisedWorkDecayStart1 = 866000; // block where weight decay starts
        consensus.nBlockAlgoNormalisedWorkDecayStart2 = 932000; // block where weight decay starts
        consensus.nGeoAvgWork_Start = 1400000;
        consensus.nFork1MinBlock = 1764000; // minimum block height where fork 1 takes effect (algo switch, seq algo count change)

        consensus.MIP2Height = 2328480;

        // Legbit parameters (used for v0.11 -> v0.14 compatability)
        consensus.nLegbitStart = 2306304;
        consensus.nLegbitStop = 2586528;

        // MIP3 parameters
        consensus.nPowTargetSpacingV3a = 2 * 60; // new target time for block spacing across all algorithms (2min)
        consensus.nPowTargetSpacingV3b = 4 * 60; // new target time for block spacing across all algorithms (4min)
        consensus.nPowTargetSpacingV3c = 8 * 60; // new target time for block spacing across all algorithms (8min)
        consensus.nLongblocks_StartV1a = 2903040; // Start of longblocks, original 3rd block halving
        consensus.nLongblocks_StartV1b = 3386880; // Start of longblocks, original 4th block halving
        consensus.nLongblocks_StartV1c = 3628800; // Start of longblocks, original 5th block halving

        // Deployment of Legacy Blocks. Once activated, keeps v0.11 nodes on the same chain. Should be the first softfork.
        consensus.vDeployments[Consensus::DEPLOYMENT_LEGBIT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_LEGBIT].nStartTime = 1507420800; // October 8th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_LEGBIT].nTimeout = 1538956800; // October 8th, 2018

        // Deployment of MIP2 (Reserve algorithm ids)
        consensus.vDeployments[Consensus::DEPLOYMENT_RESERVEALGO].bit = 4;
        consensus.vDeployments[Consensus::DEPLOYMENT_RESERVEALGO].nStartTime = 1516320000; // Jan 19th, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_RESERVEALGO].nTimeout = 1548979200; // Feb 1st, 2019

        // Deployment of MIP3 (longblocks)
        consensus.vDeployments[Consensus::DEPLOYMENT_LONGBLOCKS].bit = 5;
        consensus.vDeployments[Consensus::DEPLOYMENT_LONGBLOCKS].nStartTime = 1525132800; // May 1st, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_LONGBLOCKS].nTimeout = 1556668800; // May 1st, 2019

        /*** Upstream Chainparams ***/

        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP16Height = 100;
        consensus.BIP34Height = 100;
        consensus.BIP34Hash = uint256S("0x2ca9968704301897b956f7e326375413be505509489c06aee2b16fe73805481e");
        consensus.BIP65Height = 100; // 2ca9968704301897b956f7e326375413be505509489c06aee2b16fe73805481e
        consensus.BIP66Height = 100; // 2ca9968704301897b956f7e326375413be505509489c06aee2b16fe73805481e
        consensus.powLimit = ArithToUint256(~arith_uint256(0) >> 20);
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = consensus.nPowTargetSpacingV2; // Current value
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1512086400; // December 1st, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1543622400; // December 1st, 2018

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1513296000; // December 15th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1544832000; // December 15th, 2018

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00000631170923bb3d28727d9a8b3166ec0c5db3bc816a2be27657d6caa93942"); // 0

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xaa;
        pchMessageStart[1] = 0xff;
        pchMessageStart[2] = 0xc3;
        pchMessageStart[3] = 0x33;
        nDefaultPort = 9012;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1542051081, 346790, 0x1e0fffff);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000631170923bb3d28727d9a8b3166ec0c5db3bc816a2be27657d6caa93942"));
        assert(genesis.hashMerkleRoot == uint256S("0x8064891fa679776cc8a766f6bb10b1f7b38cf39785a09858e6cbb67577fffaa4"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as a oneshot if they dont support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,25);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,28);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,80);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x06, 0xC4, 0xAB, 0xC8};
        base58Prefixes[EXT_SECRET_KEY] = {0x06, 0xC4, 0xAB, 0xC9};

        bech32_hrp = "bad";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = {
            {
                {0, uint256S("00000631170923bb3d28727d9a8b3166ec0c5db3bc816a2be27657d6caa93942")},
            }
        };

        chainTxData = ChainTxData{
            // Data as of block 0x00000631170923bb3d28727d9a8b3166ec0c5db3bc816a2be27657d6caa93942 (height 0).
            1542051081, // * UNIX timestamp of last known number of transactions
            0,          // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0           // * estimated number of transactions per second after that timestamp
        };
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";

        /*** Badcoin Additional Chainparams ***/

        consensus.nPowTargetSpacingV1 = 30; // target time for block spacing across all algorithms
        consensus.nPowTargetSpacingV2 = 60; // new target time for block spacing across all algorithms
        consensus.nAveragingInterval = 10; // number of blocks to take the timespan of

        consensus.nStartAuxPow = 150;
        consensus.nAuxpowChainId = 0x005A; 
        consensus.fStrictChainId = false;

        consensus.nBlockTimeWarpPreventStart1 = 1000; // block where time warp 1 prevention starts
        consensus.nBlockTimeWarpPreventStart2 = 1005; // block where time warp 2 prevention starts
        consensus.nBlockTimeWarpPreventStart3 = 1010; // block where time warp 3 prevention starts
        consensus.Phase2Timespan_Start = 150; // block where 60 second target time kicks in
        consensus.nBlockDiffAdjustV2 = 150; // block where difficulty adjust V2 starts

        consensus.nMaxAdjustDown = 4; // 4% adjustment down
        consensus.nMaxAdjustUpV1 = 2; // 2% adjustment up
        consensus.nMaxAdjustUpV2 = 4; // 4% adjustment up     

        consensus.nBlockSequentialAlgoRuleStart1 = 200; // block where sequential algo rule starts
        consensus.nBlockSequentialAlgoRuleStart2 = 250; // block where sequential algo rule starts
        consensus.nBlockSequentialAlgoMaxCount1 = 6; // maximum sequential blocks of same algo
        consensus.nBlockSequentialAlgoMaxCount2 = 3; // maximum sequential blocks of same algo
        consensus.nBlockSequentialAlgoMaxCount3 = 6; // maximum sequential blocks of same algo

        consensus.nBlockAlgoWorkWeightStart = 0; // block where algo work weighting starts
        consensus.nBlockAlgoNormalisedWorkStart = 0; // block where algo combined weight starts
        consensus.nBlockAlgoNormalisedWorkDecayStart1 = 0; // block where weight decay starts
        consensus.nBlockAlgoNormalisedWorkDecayStart2 = 0; // block where weight decay starts
        consensus.nGeoAvgWork_Start = 150;
        consensus.nFork1MinBlock = 601; // minimum block height where fork 1 takes effect (algo switch, seq algo count change)

        consensus.MIP2Height = 1;

        // Legbit parameters (used for v0.11 -> v0.14 compatability)
        consensus.nLegbitStart = 8064;
        consensus.nLegbitStop = 26208;

        // MIP3 parameters
        consensus.nPowTargetSpacingV3a = 2 * 60; // new target time for block spacing across all algorithms (2min)
        consensus.nPowTargetSpacingV3b = 4 * 60; // new target time for block spacing across all algorithms (4min)
        consensus.nPowTargetSpacingV3c = 8 * 60; // new target time for block spacing across all algorithms (8min)
        consensus.nLongblocks_StartV1a = 2903040; // Start of longblocks, original 3rd block halving
        consensus.nLongblocks_StartV1b = 3386880; // Start of longblocks, original 4th block halving
        consensus.nLongblocks_StartV1c = 3628800; // Start of longblocks, original 5th block halving

        // Notice for BIP9 bit 2: Please see 'legbit' notes in validation.cpp prior to future use.
        // Deployment of Legacy Blocks. Once activated, keeps v0.11 nodes on the same chain. Should be the first softfork.
        consensus.vDeployments[Consensus::DEPLOYMENT_LEGBIT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_LEGBIT].nStartTime = 1504224000; // September 1st, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_LEGBIT].nTimeout = 1535760000; // September 1st, 2018

        consensus.vDeployments[Consensus::DEPLOYMENT_RESERVEALGO].bit = 4;
        consensus.vDeployments[Consensus::DEPLOYMENT_RESERVEALGO].nStartTime = 1516320000; // Jan 19th, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_RESERVEALGO].nTimeout = 1548979200; // Feb 1st, 2019

        // Deployment of MIP3 (longblocks)
        consensus.vDeployments[Consensus::DEPLOYMENT_LONGBLOCKS].bit = 5;
        consensus.vDeployments[Consensus::DEPLOYMENT_LONGBLOCKS].nStartTime = 1525132800; // May 1st, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_LONGBLOCKS].nTimeout = 1556668800; // May 1st, 2019

        /*** Upstream Chainparams ***/

        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP16Height = 100;
        consensus.BIP34Height = 100;
        consensus.BIP34Hash = uint256S("0x0000d23adc28e33bc05f4bee57c873ae0aab584a6a436e75ac0ed40396f6d86b");
        consensus.BIP65Height = 100; // 0x0000d23adc28e33bc05f4bee57c873ae0aab584a6a436e75ac0ed40396f6d86b
        consensus.BIP66Height = 100; // 0x0000d23adc28e33bc05f4bee57c873ae0aab584a6a436e75ac0ed40396f6d86b
        consensus.powLimit = ArithToUint256(~arith_uint256(0) >> 16);
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = consensus.nPowTargetSpacingV2; // Current value
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1504224000; // September 1st, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1535760000; // September 1st, 2018

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1506816000; // October 1st, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1538352000; // October 1st, 2018

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00000dd96875372672b943e30bd12281c75d0fed7462d486e305b09613a6d6fa"); // 1

        pchMessageStart[0] = 0xab;
        pchMessageStart[1] = 0xff;
        pchMessageStart[2] = 0xcc;
        pchMessageStart[3] = 0x33;
        nDefaultPort = 19012;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1543455632, 32852, 0x1e0fffff, "On 11/28/2018 the Badcoin Testnet Genesis was created");
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000dd96875372672b943e30bd12281c75d0fed7462d486e305b09613a6d6fa"));
        assert(genesis.hashMerkleRoot == uint256S("0x820d2d5d85a19095e16d11e11ac716b158e7481757e22f4a02e18ccef513e5d7"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,85);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,87);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,142);
        base58Prefixes[EXT_PUBLIC_KEY] = {0xA6, 0xC4, 0xAC, 0xC8};
        base58Prefixes[EXT_SECRET_KEY] = {0xA6, 0xC4, 0xAC, 0xC9};

        bech32_hrp = "badt";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;


        checkpointData = {
            {
                {   0, uint256S("00000dd96875372672b943e30bd12281c75d0fed7462d486e305b09613a6d6fa")},
            }
        };

        chainTxData = ChainTxData{
            // Data as of block 0x00000dd96875372672b943e30bd12281c75d0fed7462d486e305b09613a6d6fa (height 0)
            1543455632,
            0,
            0.02
        };

    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";

        /*** Badcoin Additional Chainparams ***/

        consensus.nPowTargetSpacingV1 = 30; // target time for block spacing across all algorithms
        consensus.nPowTargetSpacingV2 = 60; // new target time for block spacing across all algorithms
        consensus.nAveragingInterval = 10; // number of blocks to take the timespan of

        consensus.nStartAuxPow = 150;
        consensus.nAuxpowChainId = 0x005A;
        consensus.fStrictChainId = false;

        consensus.nBlockTimeWarpPreventStart1 = 1000; // block where time warp 1 prevention starts
        consensus.nBlockTimeWarpPreventStart2 = 1005; // block where time warp 2 prevention starts
        consensus.nBlockTimeWarpPreventStart3 = 1010; // block where time warp 3 prevention starts
        consensus.Phase2Timespan_Start = 150; // block where 60 second target time kicks in
        consensus.nBlockDiffAdjustV2 = 150; // block where difficulty adjust V2 starts

        consensus.nMaxAdjustDown = 4; // 4% adjustment down
        consensus.nMaxAdjustUpV1 = 2; // 2% adjustment up
        consensus.nMaxAdjustUpV2 = 4; // 4% adjustment up

        consensus.nBlockSequentialAlgoRuleStart1 = 200; // block where sequential algo rule starts
        consensus.nBlockSequentialAlgoRuleStart2 = 250; // block where sequential algo rule starts
        consensus.nBlockSequentialAlgoMaxCount1 = 6; // maximum sequential blocks of same algo
        consensus.nBlockSequentialAlgoMaxCount2 = 3; // maximum sequential blocks of same algo
        consensus.nBlockSequentialAlgoMaxCount3 = 6; // maximum sequential blocks of same algo

        consensus.nBlockAlgoWorkWeightStart = 0; // block where algo work weighting starts
        consensus.nBlockAlgoNormalisedWorkStart = 0; // block where algo combined weight starts
        consensus.nBlockAlgoNormalisedWorkDecayStart1 = 0; // block where weight decay starts
        consensus.nBlockAlgoNormalisedWorkDecayStart2 = 0; // block where weight decay starts
        consensus.nGeoAvgWork_Start = 0;
        consensus.nFork1MinBlock = 601; // minimum block height where fork 1 takes effect (algo switch, seq algo count change)

        consensus.MIP2Height = 1;

        // Legbit parameters (used for v0.11 -> v0.14 compatability)
        consensus.nLegbitStart = 0;
        consensus.nLegbitStop = 0;

        // MIP3 parameters
        consensus.nPowTargetSpacingV3a = 2 * 60; // new target time for block spacing across all algorithms (2min)
        consensus.nPowTargetSpacingV3b = 4 * 60; // new target time for block spacing across all algorithms (4min)
        consensus.nPowTargetSpacingV3c = 8 * 60; // new target time for block spacing across all algorithms (8min)
        consensus.nLongblocks_StartV1a = 450; // Start of longblocks, original 3rd block halving
        consensus.nLongblocks_StartV1b = 525; // Start of longblocks, original 4th block halving
        consensus.nLongblocks_StartV1c = 562; // Start of longblocks, original 5th block halving

        consensus.vDeployments[Consensus::DEPLOYMENT_LEGBIT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_LEGBIT].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_LEGBIT].nTimeout = 999999999999ULL;

        // Deployment of MIP2 (Reserve algorithm ids)
        consensus.vDeployments[Consensus::DEPLOYMENT_RESERVEALGO].bit = 4;
        consensus.vDeployments[Consensus::DEPLOYMENT_RESERVEALGO].nStartTime = 0; 
        consensus.vDeployments[Consensus::DEPLOYMENT_RESERVEALGO].nTimeout = 999999999999ULL;

        // Deployment of MIP3 (longblocks)
        consensus.vDeployments[Consensus::DEPLOYMENT_LONGBLOCKS].bit = 5;
        consensus.vDeployments[Consensus::DEPLOYMENT_LONGBLOCKS].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_LONGBLOCKS].nTimeout = 999999999999ULL;

        /*** Upstream Chainparams ***/

        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP16Height = 0; // always enforce P2SH BIP16 on regtest
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = consensus.nPowTargetSpacingV2; // Current value
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xac;
        pchMessageStart[1] = 0xff;
        pchMessageStart[2] = 0xcc;
        pchMessageStart[3] = 0x36;
        nDefaultPort = 18445;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1543452687, 0, 0x207fffff, "On 11/28/2018 the Badcoin Regtest Genesis was created");
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x50d433aa090ff0dab2bee28707938394f2f817943b466b416bc3f80faa953730"));
        assert(genesis.hashMerkleRoot == uint256S("0x7a3655d89ca64bf6d5573ea33c73be98902724fc846b24a5ce8cf23af2359ff1"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = {
            {
                {0, uint256S("50d433aa090ff0dab2bee28707938394f2f817943b466b416bc3f80faa953730")},
            }
        };

        chainTxData = ChainTxData{
            1543452687,
            1,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,120);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,130);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,140);
        base58Prefixes[EXT_PUBLIC_KEY] = {0xB6, 0xC4, 0xAD, 0xC8};
        base58Prefixes[EXT_SECRET_KEY] = {0xB6, 0xC4, 0xAD, 0xC9};

        bech32_hrp = "bdrg";
    }
};

static std::unique_ptr<CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    globalChainParams->UpdateVersionBitsParameters(d, nStartTime, nTimeout);
}
