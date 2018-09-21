// Copyright (c) 2014-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "validation.h"
#include "net.h"

#include "test/test_bitcoin.h"

#include <boost/signals2/signal.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(main_tests, TestingSetup)

static void TestBlockSubsidyHalvings(const Consensus::Params& consensusParams)
{
    int maxHalvings = 64;
    CAmount nInitialSubsidy = 1000 * COIN;
    CAmount nFinalSubsidy = 1 * COIN;

    CAmount nPreviousSubsidy = nInitialSubsidy * 2; // for height == 0
    BOOST_CHECK_EQUAL(nPreviousSubsidy, nInitialSubsidy * 2);
    for (int nHalvings = 0; nHalvings < maxHalvings; nHalvings++) {
        // Myriadcoin: nSubsidyHalvingInterval has been altered in 3 stages for MIP3-longblocks
        //int nHeight = nHalvings * consensusParams.nSubsidyHalvingInterval;
        int nHeight = 0;
        if (nHalvings<=3) {
            nHeight = nHalvings * consensusParams.nSubsidyHalvingInterval;
        } else if (nHalvings==4) {
            nHeight = 3 * consensusParams.nSubsidyHalvingInterval;
            nHeight += consensusParams.nSubsidyHalvingIntervalV2a;
        } else if (nHalvings==5) {
            nHeight = 3 * consensusParams.nSubsidyHalvingInterval;
            nHeight += consensusParams.nSubsidyHalvingIntervalV2a;
            nHeight += consensusParams.nSubsidyHalvingIntervalV2b;
        } else if (nHalvings>=6) {
            nHeight = 3 * consensusParams.nSubsidyHalvingInterval;
            nHeight += consensusParams.nSubsidyHalvingIntervalV2a;
            nHeight += consensusParams.nSubsidyHalvingIntervalV2b;
            nHeight += (nHalvings - 5) * consensusParams.nSubsidyHalvingIntervalV2c;
        }
        CAmount nSubsidy = GetBlockSubsidy(nHeight, consensusParams);
        BOOST_CHECK(nSubsidy <= nInitialSubsidy);
        if (nHalvings < 13) {
            // Myriadcoin: nSubidy is held for block halvings 3-5 and block time increased for MIP3-longblocks
            //BOOST_CHECK_EQUAL(nSubsidy, nPreviousSubsidy / 2);
            if (nHalvings==3 || nHalvings==4 || nHalvings==5)
                BOOST_CHECK_EQUAL(nSubsidy, nPreviousSubsidy);
            else if (nHalvings==12) // Myriadcoin: 4 sat rounding error on the last halving before tail emission
                BOOST_CHECK_EQUAL(nSubsidy, 195312496);
            else
                BOOST_CHECK_EQUAL(nSubsidy, nPreviousSubsidy / 2);
        } else {
            BOOST_CHECK_EQUAL(nSubsidy, nFinalSubsidy);
        }
        nPreviousSubsidy = nSubsidy;
    }
    BOOST_CHECK_EQUAL(GetBlockSubsidy(maxHalvings * consensusParams.nSubsidyHalvingInterval, consensusParams), nFinalSubsidy);
}

/* Myriadcoin: we don't test other intervals due to MIP3-longblocks
static void TestBlockSubsidyHalvings(int nSubsidyHalvingInterval)
{
    Consensus::Params consensusParams;
    consensusParams.nSubsidyHalvingInterval = nSubsidyHalvingInterval;
    TestBlockSubsidyHalvings(consensusParams);
}
*/

BOOST_AUTO_TEST_CASE(block_subsidy_test)
{
    TestBlockSubsidyHalvings(Params(CBaseChainParams::MAIN).GetConsensus()); // As in main
    TestBlockSubsidyHalvings(Params(CBaseChainParams::TESTNET).GetConsensus()); // As in testnet
    TestBlockSubsidyHalvings(Params(CBaseChainParams::REGTEST).GetConsensus()); // As in regtest
    // Myriadcoin: we don't test other intervals due to MIP3-longblocks
    //TestBlockSubsidyHalvings(150); // As in regtest
    //TestBlockSubsidyHalvings(1000); // Just another interval
}

BOOST_AUTO_TEST_CASE(subsidy_limit_test)
{
    const Consensus::Params& consensusParams = Params(CBaseChainParams::MAIN).GetConsensus();
    CAmount nSum = 0;
    // Myriadcoin: step must be integer factor of our MIP3 120960 halving intervals, all interval steps are divisible by 945
    for (int nHeight = 0; nHeight < 14000000; nHeight += 945) {
        CAmount nSubsidy = GetBlockSubsidy(nHeight, consensusParams);
        BOOST_CHECK(nSubsidy <= 1000 * COIN);
        nSum += nSubsidy * 945;
        BOOST_CHECK(MoneyRange(nSum));
    }
    BOOST_CHECK_EQUAL(nSum, 194452744499516160ULL);
}

bool ReturnFalse() { return false; }
bool ReturnTrue() { return true; }

BOOST_AUTO_TEST_CASE(test_combiner_all)
{
    boost::signals2::signal<bool (), CombinerAll> Test;
    BOOST_CHECK(Test());
    Test.connect(&ReturnFalse);
    BOOST_CHECK(!Test());
    Test.connect(&ReturnTrue);
    BOOST_CHECK(!Test());
    Test.disconnect(&ReturnFalse);
    BOOST_CHECK(Test());
    Test.disconnect(&ReturnTrue);
    BOOST_CHECK(Test());
}
BOOST_AUTO_TEST_SUITE_END()
