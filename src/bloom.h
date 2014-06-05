


#ifndef BITCOIN_BLOOM_H
#define BITCOIN_BLOOM_H

#include <vector>

#include "uint256.h"
#include "serialize.h"

class COutPoint;
class CTransaction;



static const unsigned int MAX_HASH_FUNCS = 50;



enum bloomflags
{
    BLOOM_UPDATE_NONE = 0,
    BLOOM_UPDATE_ALL = 1,

    BLOOM_UPDATE_P2PUBKEY_ONLY = 2,
    BLOOM_UPDATE_MASK = 3,
};


class CBloomFilter
{
private:
    std::vector<unsigned char> vData;
    bool isFull;
    bool isEmpty;
    unsigned int nHashFuncs;
    unsigned int nTweak;
    unsigned char nFlags;

    unsigned int Hash(unsigned int nHashNum, const std::vector<unsigned char>& vDataToHash) const;

public:







    CBloomFilter(unsigned int nElements, double nFPRate, unsigned int nTweak, unsigned char nFlagsIn);
    CBloomFilter() : isFull(true) {}

    IMPLEMENT_SERIALIZE
    (
        READWRITE(vData);
        READWRITE(nHashFuncs);
        READWRITE(nTweak);
        READWRITE(nFlags);
    )

    void insert(const std::vector<unsigned char>& vKey);
    void insert(const COutPoint& outpoint);
    void insert(const uint256& hash);

    bool contains(const std::vector<unsigned char>& vKey) const;
    bool contains(const COutPoint& outpoint) const;
    bool contains(const uint256& hash) const;



    bool IsWithinSizeConstraints() const;


    bool IsRelevantAndUpdate(const CTransaction& tx, const uint256& hash);


    void UpdateEmptyFull();
};

#endif 
