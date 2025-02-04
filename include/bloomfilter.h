#pragma once 
#include <string>
#include <cstdint>
#include <vector>
#include <cstddef>  
#include "../external/MurmurHash3/murmurhash3.h"

class BloomFilter;

class BloomFilter {
private:
    std::size_t positionBits;
    bool isSet(uint64_t index) const;
    std::vector<bool> encodePosition(uint64_t position) const;

protected:
    std::vector<bool> presenceBitset;

    std::vector<std::vector<bool>> positionBitsets;
    static uint64_t combine128to64(const uint8_t hash128[16]);
    std::size_t bitArraySize; 
    std::size_t chunkCount;
public:
    int numHashCount;
    virtual uint64_t generateHash(const std::string& item, int i, int seed = 0) const;
    BloomFilter(std::size_t elementsToEncode, double falsePositiveRate, int positionBits = 1);
    void addPresence(const std::string& item, int seed = 0);
    bool mightContain(const std::string& item, int seed = 0) const;
    void addPosition(const std::string& item, uint64_t position, int seed = 0);
    uint64_t getPosition(const std::string& item, int seed = 0) const;
    std::pair<std::vector<uint64_t>, std::vector<int>> returnPartialCollisionIndex(
        const std::vector<uint64_t>& indexes) const;
    virtual std::size_t getSize() const;
    const std::vector<bool>& getBitArray() const;
    static std::size_t calculateBitArraySize(std::size_t elementsToEncode, double falsePositiveRate);
    static int calculateOptimalHashNum(std::size_t elementsToEncode, std::size_t bitArraySize);
    static uint64_t binarySeqToDecimal(const std::vector<int>& bits);
    bool add(const std::string& item, uint64_t position, int seed = 0);
};