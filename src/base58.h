













#ifndef BITCOIN_BASE58_H
#define BITCOIN_BASE58_H

#include <string>
#include <vector>

#include "bignum.h"
#include "key.h"
#include "script.h"
#include "allocators.h"

static const char* pszBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";


inline std::string EncodeBase58(const unsigned char* pbegin, const unsigned char* pend)
{
    CAutoBN_CTX pctx;
    CBigNum bn58 = 58;
    CBigNum bn0 = 0;



    std::vector<unsigned char> vchTmp(pend-pbegin+1, 0);
    reverse_copy(pbegin, pend, vchTmp.begin());


    CBigNum bn;
    bn.setvch(vchTmp);


    std::string str;


    str.reserve((pend - pbegin) * 138 / 100 + 1);
    CBigNum dv;
    CBigNum rem;
    while (bn > bn0)
    {
        if (!BN_div(&dv, &rem, &bn, &bn58, pctx))
            throw bignum_error("EncodeBase58 : BN_div failed");
        bn = dv;
        unsigned int c = rem.getulong();
        str += pszBase58[c];
    }


    for (const unsigned char* p = pbegin; p < pend && *p == 0; p++)
        str += pszBase58[0];


    reverse(str.begin(), str.end());
    return str;
}


inline std::string EncodeBase58(const std::vector<unsigned char>& vch)
{
    return EncodeBase58(&vch[0], &vch[0] + vch.size());
}



inline bool DecodeBase58(const char* psz, std::vector<unsigned char>& vchRet)
{
    CAutoBN_CTX pctx;
    vchRet.clear();
    CBigNum bn58 = 58;
    CBigNum bn = 0;
    CBigNum bnChar;
    while (isspace(*psz))
        psz++;


    for (const char* p = psz; *p; p++)
    {
        const char* p1 = strchr(pszBase58, *p);
        if (p1 == NULL)
        {
            while (isspace(*p))
                p++;
            if (*p != '\0')
                return false;
            break;
        }
        bnChar.setulong(p1 - pszBase58);
        if (!BN_mul(&bn, &bn, &bn58, pctx))
            throw bignum_error("DecodeBase58 : BN_mul failed");
        bn += bnChar;
    }


    std::vector<unsigned char> vchTmp = bn.getvch();


    if (vchTmp.size() >= 2 && vchTmp.end()[-1] == 0 && vchTmp.end()[-2] >= 0x80)
        vchTmp.erase(vchTmp.end()-1);


    int nLeadingZeros = 0;
    for (const char* p = psz; *p == pszBase58[0]; p++)
        nLeadingZeros++;
    vchRet.assign(nLeadingZeros + vchTmp.size(), 0);


    reverse_copy(vchTmp.begin(), vchTmp.end(), vchRet.end() - vchTmp.size());
    return true;
}



inline bool DecodeBase58(const std::string& str, std::vector<unsigned char>& vchRet)
{
    return DecodeBase58(str.c_str(), vchRet);
}





inline std::string EncodeBase58Check(const std::vector<unsigned char>& vchIn)
{

    std::vector<unsigned char> vch(vchIn);
    uint256 hash = Hash(vch.begin(), vch.end());
    vch.insert(vch.end(), (unsigned char*)&hash, (unsigned char*)&hash + 4);
    return EncodeBase58(vch);
}



inline bool DecodeBase58Check(const char* psz, std::vector<unsigned char>& vchRet)
{
    if (!DecodeBase58(psz, vchRet))
        return false;
    if (vchRet.size() < 4)
    {
        vchRet.clear();
        return false;
    }
    uint256 hash = Hash(vchRet.begin(), vchRet.end()-4);
    if (memcmp(&hash, &vchRet.end()[-4], 4) != 0)
    {
        vchRet.clear();
        return false;
    }
    vchRet.resize(vchRet.size()-4);
    return true;
}



inline bool DecodeBase58Check(const std::string& str, std::vector<unsigned char>& vchRet)
{
    return DecodeBase58Check(str.c_str(), vchRet);
}






class CBase58Data
{
protected:

    unsigned char nVersion;


    typedef std::vector<unsigned char, zero_after_free_allocator<unsigned char> > vector_uchar;
    vector_uchar vchData;

    CBase58Data()
    {
        nVersion = 0;
        vchData.clear();
    }

    void SetData(int nVersionIn, const void* pdata, size_t nSize)
    {
        nVersion = nVersionIn;
        vchData.resize(nSize);
        if (!vchData.empty())
            memcpy(&vchData[0], pdata, nSize);
    }

    void SetData(int nVersionIn, const unsigned char *pbegin, const unsigned char *pend)
    {
        SetData(nVersionIn, (void*)pbegin, pend - pbegin);
    }

public:
    bool SetString(const char* psz)
    {
        std::vector<unsigned char> vchTemp;
        DecodeBase58Check(psz, vchTemp);
        if (vchTemp.empty())
        {
            vchData.clear();
            nVersion = 0;
            return false;
        }
        nVersion = vchTemp[0];
        vchData.resize(vchTemp.size() - 1);
        if (!vchData.empty())
            memcpy(&vchData[0], &vchTemp[1], vchData.size());
        OPENSSL_cleanse(&vchTemp[0], vchData.size());
        return true;
    }

    bool SetString(const std::string& str)
    {
        return SetString(str.c_str());
    }

    std::string ToString() const
    {
        std::vector<unsigned char> vch(1, nVersion);
        vch.insert(vch.end(), vchData.begin(), vchData.end());
        return EncodeBase58Check(vch);
    }

    int CompareTo(const CBase58Data& b58) const
    {
        if (nVersion < b58.nVersion) return -1;
        if (nVersion > b58.nVersion) return  1;
        if (vchData < b58.vchData)   return -1;
        if (vchData > b58.vchData)   return  1;
        return 0;
    }

    bool operator==(const CBase58Data& b58) const { return CompareTo(b58) == 0; }
    bool operator<=(const CBase58Data& b58) const { return CompareTo(b58) <= 0; }
    bool operator>=(const CBase58Data& b58) const { return CompareTo(b58) >= 0; }
    bool operator< (const CBase58Data& b58) const { return CompareTo(b58) <  0; }
    bool operator> (const CBase58Data& b58) const { return CompareTo(b58) >  0; }
};


class CBitcoinAddress;
class CBitcoinAddressVisitor : public boost::static_visitor<bool>
{
private:
    CBitcoinAddress *addr;
public:
    CBitcoinAddressVisitor(CBitcoinAddress *addrIn) : addr(addrIn) { }
    bool operator()(const CKeyID &id) const;
    bool operator()(const CScriptID &id) const;
    bool operator()(const CNoDestination &no) const;
};

class CBitcoinAddress : public CBase58Data
{
public:
    enum
    {

        SCRIPT_ADDRESS = 5,
        PUBKEY_ADDRESS_TEST = 111,
        SCRIPT_ADDRESS_TEST = 196,
    };

    bool Set(const CKeyID &id) {
        SetData(fTestNet ? PUBKEY_ADDRESS_TEST : PUBKEY_ADDRESS, &id, 20);
        return true;
    }

    bool Set(const CScriptID &id) {
        SetData(fTestNet ? SCRIPT_ADDRESS_TEST : SCRIPT_ADDRESS, &id, 20);
        return true;
    }

    bool Set(const CTxDestination &dest)
    {
        return boost::apply_visitor(CBitcoinAddressVisitor(this), dest);
    }

    bool IsValid() const
    {
        unsigned int nExpectedSize = 20;
        bool fExpectTestNet = false;
        switch(nVersion)
        {
            case PUBKEY_ADDRESS:

                fExpectTestNet = false;
                break;
            case SCRIPT_ADDRESS:

                fExpectTestNet = false;
                break;

            case PUBKEY_ADDRESS_TEST:
                nExpectedSize = 20;
                fExpectTestNet = true;
                break;
            case SCRIPT_ADDRESS_TEST:
                nExpectedSize = 20;
                fExpectTestNet = true;
                break;

            default:
                return false;
        }
        return fExpectTestNet == fTestNet && vchData.size() == nExpectedSize;
    }

    CBitcoinAddress()
    {
    }

    CBitcoinAddress(const CTxDestination &dest)
    {
        Set(dest);
    }

    CBitcoinAddress(const std::string& strAddress)
    {
        SetString(strAddress);
    }

    CBitcoinAddress(const char* pszAddress)
    {
        SetString(pszAddress);
    }

    CTxDestination Get() const {
        if (!IsValid())
            return CNoDestination();
        switch (nVersion) {
        case PUBKEY_ADDRESS:
        case PUBKEY_ADDRESS_TEST: {
            uint160 id;
            memcpy(&id, &vchData[0], 20);
            return CKeyID(id);
        }
        case SCRIPT_ADDRESS:
        case SCRIPT_ADDRESS_TEST: {
            uint160 id;
            memcpy(&id, &vchData[0], 20);
            return CScriptID(id);
        }
        }
        return CNoDestination();
    }

    bool GetKeyID(CKeyID &keyID) const {
        if (!IsValid())
            return false;
        switch (nVersion) {
        case PUBKEY_ADDRESS:
        case PUBKEY_ADDRESS_TEST: {
            uint160 id;
            memcpy(&id, &vchData[0], 20);
            keyID = CKeyID(id);
            return true;
        }
        default: return false;
        }
    }

    bool IsScript() const {
        if (!IsValid())
            return false;
        switch (nVersion) {
        case SCRIPT_ADDRESS:
        case SCRIPT_ADDRESS_TEST: {
            return true;
        }
        default: return false;
        }
    }
};

bool inline CBitcoinAddressVisitor::operator()(const CKeyID &id) const         { return addr->Set(id); }
bool inline CBitcoinAddressVisitor::operator()(const CScriptID &id) const      { return addr->Set(id); }
bool inline CBitcoinAddressVisitor::operator()(const CNoDestination &id) const { return false; }


class CBitcoinSecret : public CBase58Data
{
public:
    enum
    {
        PRIVKEY_ADDRESS = CBitcoinAddress::PUBKEY_ADDRESS + 128,
        PRIVKEY_ADDRESS_TEST = CBitcoinAddress::PUBKEY_ADDRESS_TEST + 128,
    };

    void SetKey(const CKey& vchSecret)
    {
        assert(vchSecret.IsValid());
        SetData(fTestNet ? PRIVKEY_ADDRESS_TEST : PRIVKEY_ADDRESS, vchSecret.begin(), vchSecret.size());
        if (vchSecret.IsCompressed())
            vchData.push_back(1);
    }

    CKey GetKey()
    {
        CKey ret;
        ret.Set(&vchData[0], &vchData[32], vchData.size() > 32 && vchData[32] == 1);
        return ret;
    }

    bool IsValid() const
    {
        bool fExpectTestNet = false;
        switch(nVersion)
        {
            case PRIVKEY_ADDRESS:
                break;

            case PRIVKEY_ADDRESS_TEST:
                fExpectTestNet = true;
                break;

            default:
                return false;
        }
        return fExpectTestNet == fTestNet && (vchData.size() == 32 || (vchData.size() == 33 && vchData[32] == 1));
    }

    bool SetString(const char* pszSecret)
    {
        return CBase58Data::SetString(pszSecret) && IsValid();
    }

    bool SetString(const std::string& strSecret)
    {
        return SetString(strSecret.c_str());
    }

    CBitcoinSecret(const CKey& vchSecret)
    {
        SetKey(vchSecret);
    }

    CBitcoinSecret()
    {
    }
};


