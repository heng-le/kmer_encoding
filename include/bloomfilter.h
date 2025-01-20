#pragma once 
#include <string>
#include <cstdint>
#include <vector>
#include <cstddef>  
#include "../external/MurmurHash3/murmurhash3.h"

class BloomFilter;

class BloomFilter {
private:
    // This is a bitset for presence encoding 
    std::vector<bool> presenceBitset;

    std::vector<std::vector<bool>> positionBitsets;

    std::size_t bitArraySize;


    std::size_t positionBits;
    std::size_t chunkCount;
    bool isSet(uint64_t index) const;
    std::vector<bool> encodePosition(uint64_t position) const;


protected:
    static uint64_t combine128to64(const uint8_t hash128[16]);
public:
    int numHashCount;
    uint64_t generateHash(const std::string& item, int i, int seed = 0) const;
    BloomFilter(std::size_t elementsToEncode, double falsePositiveRate, int positionBits = 1);
    void addPresence(const std::string& item, int seed = 0);
    bool mightContain(const std::string& item, int seed = 0) const;
    void addPosition(const std::string& item, uint64_t position, int seed = 0);
    uint64_t getPosition(const std::string& item, int seed = 0) const;
    std::pair<std::vector<uint64_t>, std::vector<int>> returnPartialCollisionIndex(
        const std::vector<uint64_t>& indexes) const;
    std::size_t getSize() const;
    const std::vector<bool>& getBitArray() const;
    static std::size_t calculateBitArraySize(std::size_t elementsToEncode, double falsePositiveRate);
    static int calculateOptimalHashNum(std::size_t elementsToEncode, std::size_t bitArraySize);
    static uint64_t binarySeqToDecimal(const std::vector<int>& bits);
    bool add(const std::string& item, uint64_t position, int seed = 0);
};
