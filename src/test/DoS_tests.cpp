


#include <algorithm>


#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

#include "main.h"
#include "wallet.h"
#include "net.h"
#include "util.h"

#include <stdint.h>


extern bool AddOrphanTx(const CTransaction& tx);
extern unsigned int LimitOrphanTxSize(unsigned int nMaxOrphans);
extern std::map<uint256, CTransaction> mapOrphanTransactions;
extern std::map<uint256, std::set<uint256> > mapOrphanTransactionsByPrev;

CService ip(uint32_t i)
{
    struct in_addr s;
    s.s_addr = i;
    return CService(CNetAddr(s), GetDefaultPort());
}

BOOST_AUTO_TEST_SUITE(DoS_tests)

BOOST_AUTO_TEST_CASE(DoS_banning)
{
    CNode::ClearBanned();
    CAddress addr1(ip(0xa0b0c001));
    CNode dummyNode1(INVALID_SOCKET, addr1, "", true);

    BOOST_CHECK(CNode::IsBanned(addr1));


    CAddress addr2(ip(0xa0b0c002));
    CNode dummyNode2(INVALID_SOCKET, addr2, "", true);
    dummyNode2.Misbehaving(50);


    dummyNode2.Misbehaving(50);
    BOOST_CHECK(CNode::IsBanned(addr2));
}

BOOST_AUTO_TEST_CASE(DoS_banscore)
{
    CNode::ClearBanned();

    CAddress addr1(ip(0xa0b0c001));
    CNode dummyNode1(INVALID_SOCKET, addr1, "", true);
    dummyNode1.Misbehaving(100);
    BOOST_CHECK(!CNode::IsBanned(addr1));
    dummyNode1.Misbehaving(10);
    BOOST_CHECK(!CNode::IsBanned(addr1));
    dummyNode1.Misbehaving(1);
    BOOST_CHECK(CNode::IsBanned(addr1));
    mapArgs.erase("-banscore");
}

BOOST_AUTO_TEST_CASE(DoS_bantime)
{
    CNode::ClearBanned();
    int64 nStartTime = GetTime();


    CAddress addr(ip(0xa0b0c001));
    CNode dummyNode(INVALID_SOCKET, addr, "", true);

    dummyNode.Misbehaving(100);
    BOOST_CHECK(CNode::IsBanned(addr));

    SetMockTime(nStartTime+60*60);
    BOOST_CHECK(CNode::IsBanned(addr));

    SetMockTime(nStartTime+60*60*24+1);
    BOOST_CHECK(!CNode::IsBanned(addr));
}

static bool CheckNBits(unsigned int nbits1, int64 time1, unsigned int nbits2, int64 time2)\
{
    if (time1 > time2)
        return CheckNBits(nbits2, time2, nbits1, time1);
    int64 deltaTime = time2-time1;

    CBigNum required;
    required.SetCompact(ComputeMinWork(nbits1, deltaTime));
    CBigNum have;
    have.SetCompact(nbits2);
    return (have <= required);
}

BOOST_AUTO_TEST_CASE(DoS_checknbits)
{




    typedef std::map<int64, unsigned int> BlockData;
    BlockData chainData =
        map_list_of(1239852051,486604799)(1262749024,486594666)
        (1279305360,469854461)(1280200847,469830746)(1281678674,469809688)
        (1296207707,453179945)(1302624061,453036989)(1309640330,437004818)
        (1313172719,436789733);



    BOOST_FOREACH(const BlockData::value_type& i, chainData)
    {
        BOOST_FOREACH(const BlockData::value_type& j, chainData)
        {
            BOOST_CHECK(CheckNBits(i.second, i.first, j.second, j.first));
        }
    }


    BlockData::value_type firstcheck = *(chainData.begin());
    BlockData::value_type lastcheck = *(chainData.rbegin());



    BOOST_CHECK(!CheckNBits(firstcheck.second, lastcheck.first+60*10, lastcheck.second, lastcheck.first));
    BOOST_CHECK(!CheckNBits(firstcheck.second, lastcheck.first+60*60*24*14, lastcheck.second, lastcheck.first));


    BOOST_CHECK(CheckNBits(firstcheck.second, lastcheck.first+60*60*24*365*4, lastcheck.second, lastcheck.first));
}

CTransaction RandomOrphan()
{
    std::map<uint256, CTransaction>::iterator it;
    it = mapOrphanTransactions.lower_bound(GetRandHash());
    if (it == mapOrphanTransactions.end())
        it = mapOrphanTransactions.begin();
    return it->second;
}

BOOST_AUTO_TEST_CASE(DoS_mapOrphans)
{
    CKey key;
    key.MakeNewKey(true);
    CBasicKeyStore keystore;
    keystore.AddKey(key);


    for (int i = 0; i < 50; i++)
    {
        CTransaction tx;
        tx.vin.resize(1);
        tx.vin[0].prevout.n = 0;
        tx.vin[0].prevout.hash = GetRandHash();
        tx.vin[0].scriptSig << OP_1;
        tx.vout.resize(1);
        tx.vout[0].nValue = 1*CENT;
        tx.vout[0].scriptPubKey.SetDestination(key.GetPubKey().GetID());

        AddOrphanTx(tx);
    }


    for (int i = 0; i < 50; i++)
    {
        CTransaction txPrev = RandomOrphan();

        CTransaction tx;
        tx.vin.resize(1);
        tx.vin[0].prevout.n = 0;
        tx.vin[0].prevout.hash = txPrev.GetHash();
        tx.vout.resize(1);
        tx.vout[0].nValue = 1*CENT;
        tx.vout[0].scriptPubKey.SetDestination(key.GetPubKey().GetID());
        SignSignature(keystore, txPrev, tx, 0);

        AddOrphanTx(tx);
    }


    for (int i = 0; i < 10; i++)
    {
        CTransaction txPrev = RandomOrphan();

        CTransaction tx;
        tx.vout.resize(1);
        tx.vout[0].nValue = 1*CENT;
        tx.vout[0].scriptPubKey.SetDestination(key.GetPubKey().GetID());
        tx.vin.resize(500);
        for (unsigned int j = 0; j < tx.vin.size(); j++)
        {
            tx.vin[j].prevout.n = j;
            tx.vin[j].prevout.hash = txPrev.GetHash();
        }
        SignSignature(keystore, txPrev, tx, 0);


        for (unsigned int j = 1; j < tx.vin.size(); j++)
            tx.vin[j].scriptSig = tx.vin[0].scriptSig;

        BOOST_CHECK(!AddOrphanTx(tx));
    }


    LimitOrphanTxSize(40);
    BOOST_CHECK(mapOrphanTransactions.size() <= 40);
    LimitOrphanTxSize(10);
    BOOST_CHECK(mapOrphanTransactions.size() <= 10);
    LimitOrphanTxSize(0);
    BOOST_CHECK(mapOrphanTransactions.empty());
    BOOST_CHECK(mapOrphanTransactionsByPrev.empty());
}

BOOST_AUTO_TEST_CASE(DoS_checkSig)
{


    CKey key;
    key.MakeNewKey(true);
    CBasicKeyStore keystore;
    keystore.AddKey(key);
    unsigned int flags = SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_STRICTENC;


    static const int NPREV=100;
    CTransaction orphans[NPREV];
    for (int i = 0; i < NPREV; i++)
    {
        CTransaction& tx = orphans[i];
        tx.vin.resize(1);
        tx.vin[0].prevout.n = 0;
        tx.vin[0].prevout.hash = GetRandHash();
        tx.vin[0].scriptSig << OP_1;
        tx.vout.resize(1);
        tx.vout[0].nValue = 1*CENT;
        tx.vout[0].scriptPubKey.SetDestination(key.GetPubKey().GetID());

        AddOrphanTx(tx);
    }


    CTransaction tx;
    tx.vout.resize(1);
    tx.vout[0].nValue = 1*CENT;
    tx.vout[0].scriptPubKey.SetDestination(key.GetPubKey().GetID());
    tx.vin.resize(NPREV);
    for (unsigned int j = 0; j < tx.vin.size(); j++)
    {
        tx.vin[j].prevout.n = 0;
        tx.vin[j].prevout.hash = orphans[j].GetHash();
    }

    boost::posix_time::ptime mst1 = boost::posix_time::microsec_clock::local_time();
    for (unsigned int j = 0; j < tx.vin.size(); j++)
        BOOST_CHECK(SignSignature(keystore, orphans[j], tx, j));
    boost::posix_time::ptime mst2 = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration msdiff = mst2 - mst1;
    long nOneValidate = msdiff.total_milliseconds();
    if (fDebug) printf("DoS_Checksig sign: %ld\n", nOneValidate);





    mst1 = boost::posix_time::microsec_clock::local_time();
    for (unsigned int i = 0; i < 5; i++)
        for (unsigned int j = 0; j < tx.vin.size(); j++)
            BOOST_CHECK(VerifySignature(CCoins(orphans[j], MEMPOOL_HEIGHT), tx, j, flags, SIGHASH_ALL));
    mst2 = boost::posix_time::microsec_clock::local_time();
    msdiff = mst2 - mst1;
    long nManyValidate = msdiff.total_milliseconds();
    if (fDebug) printf("DoS_Checksig five: %ld\n", nManyValidate);

    BOOST_CHECK_MESSAGE(nManyValidate < nOneValidate, "Signature cache timing failed");


    CScript save = tx.vin[0].scriptSig;
    tx.vin[0].scriptSig = CScript();
    BOOST_CHECK(!VerifySignature(CCoins(orphans[0], MEMPOOL_HEIGHT), tx, 0, flags, SIGHASH_ALL));
    tx.vin[0].scriptSig = save;


    std::swap(tx.vin[0].scriptSig, tx.vin[1].scriptSig);
    BOOST_CHECK(!VerifySignature(CCoins(orphans[0], MEMPOOL_HEIGHT), tx, 0, flags, SIGHASH_ALL));
    BOOST_CHECK(!VerifySignature(CCoins(orphans[1], MEMPOOL_HEIGHT), tx, 1, flags, SIGHASH_ALL));
    std::swap(tx.vin[0].scriptSig, tx.vin[1].scriptSig);


    mapArgs["-maxsigcachesize"] = "10";

    CScript oldSig = tx.vin[0].scriptSig;
    BOOST_CHECK(SignSignature(keystore, orphans[0], tx, 0));
    BOOST_CHECK(tx.vin[0].scriptSig != oldSig);
    for (unsigned int j = 0; j < tx.vin.size(); j++)
        BOOST_CHECK(VerifySignature(CCoins(orphans[j], MEMPOOL_HEIGHT), tx, j, flags, SIGHASH_ALL));
    mapArgs.erase("-maxsigcachesize");

    LimitOrphanTxSize(0);
}

BOOST_AUTO_TEST_SUITE_END()
