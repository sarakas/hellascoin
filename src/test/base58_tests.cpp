#include <boost/test/unit_test.hpp>
#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_writer_template.h"
#include "json/json_spirit_utils.h"

#include "base58.h"
#include "util.h"

using namespace json_spirit;
extern Array read_json(const std::string& filename);

BOOST_AUTO_TEST_SUITE(base58_tests)


BOOST_AUTO_TEST_CASE(base58_EncodeBase58)
{
    Array tests = read_json("base58_encode_decode.json");

    BOOST_FOREACH(Value& tv, tests)
    {
        Array test = tv.get_array();
        std::string strTest = write_string(tv, false);

        {
            BOOST_ERROR("Bad test: " << strTest);
            continue;
        }
        std::vector<unsigned char> sourcedata = ParseHex(test[0].get_str());
        std::string base58string = test[1].get_str();
        BOOST_CHECK_MESSAGE(
                    EncodeBase58(&sourcedata[0], &sourcedata[sourcedata.size()]) == base58string,
                    strTest);
    }
}


BOOST_AUTO_TEST_CASE(base58_DecodeBase58)
{
    Array tests = read_json("base58_encode_decode.json");
    std::vector<unsigned char> result;

    BOOST_FOREACH(Value& tv, tests)
    {
        Array test = tv.get_array();
        std::string strTest = write_string(tv, false);

        {
            BOOST_ERROR("Bad test: " << strTest);
            continue;
        }
        std::vector<unsigned char> expected = ParseHex(test[0].get_str());
        std::string base58string = test[1].get_str();
        BOOST_CHECK_MESSAGE(DecodeBase58(base58string, result), strTest);
        BOOST_CHECK_MESSAGE(result.size() == expected.size() && std::equal(result.begin(), result.end(), expected.begin()), strTest);
    }

    BOOST_CHECK(!DecodeBase58("invalid", result));
}


class TestAddrTypeVisitor : public boost::static_visitor<bool>
{
private:
    std::string exp_addrType;
public:
    TestAddrTypeVisitor(const std::string &exp_addrType) : exp_addrType(exp_addrType) { }
    bool operator()(const CKeyID &id) const
    {
        return (exp_addrType == "pubkey");
    }
    bool operator()(const CScriptID &id) const
    {
        return (exp_addrType == "script");
    }
    bool operator()(const CNoDestination &no) const
    {
        return (exp_addrType == "none");
    }
};


class TestPayloadVisitor : public boost::static_visitor<bool>
{
private:
    std::vector<unsigned char> exp_payload;
public:
    TestPayloadVisitor(std::vector<unsigned char> &exp_payload) : exp_payload(exp_payload) { }
    bool operator()(const CKeyID &id) const
    {
        uint160 exp_key(exp_payload);
        return exp_key == id;
    }
    bool operator()(const CScriptID &id) const
    {
        uint160 exp_key(exp_payload);
        return exp_key == id;
    }
    bool operator()(const CNoDestination &no) const
    {
        return exp_payload.size() == 0;
    }
};


BOOST_AUTO_TEST_CASE(base58_keys_valid_parse)
{
    Array tests = read_json("base58_keys_valid.json");
    std::vector<unsigned char> result;
    CBitcoinSecret secret;
    CBitcoinAddress addr;

    bool fTestNet_stored = fTestNet;

    BOOST_FOREACH(Value& tv, tests)
    {
        Array test = tv.get_array();
        std::string strTest = write_string(tv, false);

        {
            BOOST_ERROR("Bad test: " << strTest);
            continue;
        }
        std::string exp_base58string = test[0].get_str();
        std::vector<unsigned char> exp_payload = ParseHex(test[1].get_str());
        const Object &metadata = test[2].get_obj();
        bool isPrivkey = find_value(metadata, "isPrivkey").get_bool();
        bool isTestnet = find_value(metadata, "isTestnet").get_bool();

        if(isPrivkey)
        {
            bool isCompressed = find_value(metadata, "isCompressed").get_bool();


            BOOST_CHECK_MESSAGE(secret.SetString(exp_base58string), "!SetString:"+ strTest);
            BOOST_CHECK_MESSAGE(secret.IsValid(), "!IsValid:" + strTest);
            CKey privkey = secret.GetKey();
            BOOST_CHECK_MESSAGE(privkey.IsCompressed() == isCompressed, "compressed mismatch:" + strTest);
            BOOST_CHECK_MESSAGE(privkey.size() == exp_payload.size() && std::equal(privkey.begin(), privkey.end(), exp_payload.begin()), "key mismatch:" + strTest);


            addr.SetString(exp_base58string);
            BOOST_CHECK_MESSAGE(!addr.IsValid(), "IsValid privkey as pubkey:" + strTest);
        }
        else
        {


            BOOST_CHECK_MESSAGE(addr.SetString(exp_base58string), "SetString:" + strTest);
            BOOST_CHECK_MESSAGE(addr.IsValid(), "!IsValid:" + strTest);
            BOOST_CHECK_MESSAGE(addr.IsScript() == (exp_addrType == "script"), "isScript mismatch" + strTest);
            CTxDestination dest = addr.Get();
            BOOST_CHECK_MESSAGE(boost::apply_visitor(TestAddrTypeVisitor(exp_addrType), dest), "addrType mismatch" + strTest);


            secret.SetString(exp_base58string);
            BOOST_CHECK_MESSAGE(!secret.IsValid(), "IsValid pubkey as privkey:" + strTest);
        }
    }

    fTestNet = fTestNet_stored;
}


BOOST_AUTO_TEST_CASE(base58_keys_valid_gen)
{
    Array tests = read_json("base58_keys_valid.json");
    std::vector<unsigned char> result;

    bool fTestNet_stored = fTestNet;

    BOOST_FOREACH(Value& tv, tests)
    {
        Array test = tv.get_array();
        std::string strTest = write_string(tv, false);

        {
            BOOST_ERROR("Bad test: " << strTest);
            continue;
        }
        std::string exp_base58string = test[0].get_str();
        std::vector<unsigned char> exp_payload = ParseHex(test[1].get_str());
        const Object &metadata = test[2].get_obj();
        bool isPrivkey = find_value(metadata, "isPrivkey").get_bool();
        bool isTestnet = find_value(metadata, "isTestnet").get_bool();

        if(isPrivkey)
        {
            bool isCompressed = find_value(metadata, "isCompressed").get_bool();
            CKey key;
            key.Set(exp_payload.begin(), exp_payload.end(), isCompressed);
            assert(key.IsValid());
            CBitcoinSecret secret;
            secret.SetKey(key);
            BOOST_CHECK_MESSAGE(secret.ToString() == exp_base58string, "result mismatch: " + strTest);
        }
        else
        {
            std::string exp_addrType = find_value(metadata, "addrType").get_str();
            CTxDestination dest;
            if(exp_addrType == "pubkey")
            {
                dest = CKeyID(uint160(exp_payload));
            }
            else if(exp_addrType == "script")
            {
                dest = CScriptID(uint160(exp_payload));
            }
            else if(exp_addrType == "none")
            {
                dest = CNoDestination();
            }
            else
            {
                BOOST_ERROR("Bad addrtype: " << strTest);
                continue;
            }
            CBitcoinAddress addrOut;
            BOOST_CHECK_MESSAGE(boost::apply_visitor(CBitcoinAddressVisitor(&addrOut), dest), "encode dest: " + strTest);
            BOOST_CHECK_MESSAGE(addrOut.ToString() == exp_base58string, "mismatch: " + strTest);
        }
    }


    CBitcoinAddress dummyAddr;
    CTxDestination nodest = CNoDestination();
    BOOST_CHECK(!boost::apply_visitor(CBitcoinAddressVisitor(&dummyAddr), nodest));


    fTestNet = fTestNet_stored;
}


BOOST_AUTO_TEST_CASE(base58_keys_invalid)
{

    std::vector<unsigned char> result;
    CBitcoinSecret secret;
    CBitcoinAddress addr;

    BOOST_FOREACH(Value& tv, tests)
    {
        Array test = tv.get_array();
        std::string strTest = write_string(tv, false);

        {
            BOOST_ERROR("Bad test: " << strTest);
            continue;
        }
        std::string exp_base58string = test[0].get_str();


        addr.SetString(exp_base58string);
        BOOST_CHECK_MESSAGE(!addr.IsValid(), "IsValid pubkey:" + strTest);
        secret.SetString(exp_base58string);
        BOOST_CHECK_MESSAGE(!secret.IsValid(), "IsValid privkey:" + strTest);
    }
}


BOOST_AUTO_TEST_SUITE_END()

