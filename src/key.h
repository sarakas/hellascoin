



#ifndef BITCOIN_KEY_H
#define BITCOIN_KEY_H

#include <vector>

#include "allocators.h"
#include "serialize.h"
#include "uint256.h"
#include "hash.h"










class CKeyID : public uint160
{
public:
    CKeyID() : uint160(0) { }
    CKeyID(const uint160 &in) : uint160(in) { }
};


class CScriptID : public uint160
{
public:
    CScriptID() : uint160(0) { }
    CScriptID(const uint160 &in) : uint160(in) { }
};


class CPubKey {
private:


    unsigned char vch[65];


    unsigned int static GetLen(unsigned char chHeader) {
        if (chHeader == 2 || chHeader == 3)
            return 33;
        if (chHeader == 4 || chHeader == 6 || chHeader == 7)
            return 65;
        return 0;
    }


    void Invalidate() {
        vch[0] = 0xFF;
    }

public:

    CPubKey() {
        Invalidate();
    }


    template<typename T>
    void Set(const T pbegin, const T pend) {
        int len = pend == pbegin ? 0 : GetLen(pbegin[0]);
        if (len && len == (pend-pbegin))
            memcpy(vch, (unsigned char*)&pbegin[0], len);
        else
            Invalidate();
    }


    template<typename T>
    CPubKey(const T pbegin, const T pend) {
        Set(pbegin, pend);
    }


    CPubKey(const std::vector<unsigned char> &vch) {
        Set(vch.begin(), vch.end());
    }


    unsigned int size() const { return GetLen(vch[0]); }
    const unsigned char *begin() const { return vch; }
    const unsigned char *end() const { return vch+size(); }
    const unsigned char &operator[](unsigned int pos) const { return vch[pos]; }


    friend bool operator==(const CPubKey &a, const CPubKey &b) {
        return a.vch[0] == b.vch[0] &&
               memcmp(a.vch, b.vch, a.size()) == 0;
    }
    friend bool operator!=(const CPubKey &a, const CPubKey &b) {
        return !(a == b);
    }
    friend bool operator<(const CPubKey &a, const CPubKey &b) {
        return a.vch[0] < b.vch[0] ||
               (a.vch[0] == b.vch[0] && memcmp(a.vch, b.vch, a.size()) < 0);
    }


    unsigned int GetSerializeSize(int nType, int nVersion) const {
        return size() + 1;
    }
    template<typename Stream> void Serialize(Stream &s, int nType, int nVersion) const {
        unsigned int len = size();
        ::WriteCompactSize(s, len);
        s.write((char*)vch, len);
    }
    template<typename Stream> void Unserialize(Stream &s, int nType, int nVersion) {
        unsigned int len = ::ReadCompactSize(s);
        if (len <= 65) {
            s.read((char*)vch, len);
        } else {

            char dummy;
            while (len--)
                s.read(&dummy, 1);
            Invalidate();
        }
    }


    CKeyID GetID() const {
        return CKeyID(Hash160(vch, vch+size()));
    }


    uint256 GetHash() const {
        return Hash(vch, vch+size());
    }


    bool IsValid() const {
        return size() > 0;
    }


    bool IsFullyValid() const;


    bool IsCompressed() const {
        return size() == 33;
    }



    bool Verify(const uint256 &hash, const std::vector<unsigned char>& vchSig) const;



    bool VerifyCompact(const uint256 &hash, const std::vector<unsigned char>& vchSig) const;


    bool RecoverCompact(const uint256 &hash, const std::vector<unsigned char>& vchSig);


    bool Decompress();
};




typedef std::vector<unsigned char, secure_allocator<unsigned char> > CPrivKey;


class CKey {
private:


    bool fValid;


    bool fCompressed;


    unsigned char vch[32];


    bool static Check(const unsigned char *vch);
public:


    CKey() : fValid(false) {
        LockObject(vch);
    }


    CKey(const CKey &secret) : fValid(secret.fValid), fCompressed(secret.fCompressed) {
        LockObject(vch);
        memcpy(vch, secret.vch, sizeof(vch));
    }


    ~CKey() {
        UnlockObject(vch);
    }


    template<typename T>
    void Set(const T pbegin, const T pend, bool fCompressedIn) {
        if (pend - pbegin != 32) {
            fValid = false;
            return;
        }
        if (Check(&pbegin[0])) {
            memcpy(vch, (unsigned char*)&pbegin[0], 32);
            fValid = true;
            fCompressed = fCompressedIn;
        } else {
            fValid = false;
        }
    }


    unsigned int size() const { return (fValid ? 32 : 0); }
    const unsigned char *begin() const { return vch; }
    const unsigned char *end() const { return vch + size(); }


    bool IsValid() const { return fValid; }


    bool IsCompressed() const { return fCompressed; }


    bool SetPrivKey(const CPrivKey &vchPrivKey, bool fCompressed);


    void MakeNewKey(bool fCompressed);



    CPrivKey GetPrivKey() const;



    CPubKey GetPubKey() const;


    bool Sign(const uint256 &hash, std::vector<unsigned char>& vchSig) const;






    bool SignCompact(const uint256 &hash, std::vector<unsigned char>& vchSig) const;
};

#endif
