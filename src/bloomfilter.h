#pragma once 
#include <string>
#include <cstdint>
#include <vector>
#include <cstddef>  
#include "../external/MurmurHash3/murmurhash3.h"

class BloomFilter;

class BloomFilter {
private:
    std::vector<bool> bitArray;
    std::size_t bitArraySize;  
    int numHashCount;

    // Single hash function using 128-bit MurmurHash3
    uint64_t generateHash(const std::string& item, int i, int seed = 0) const;

    // combining into 64-bit using XOR
    static uint64_t combine128to64(const uint8_t hash128[16]);

public:
    BloomFilter(std::size_t elementsToEncode, double falsePositiveRate);  

    void add(const std::string& item, int seed = 0);
    bool mightContain(const std::string& item, int seed = 0) const;
    std::vector<uint64_t> getHashIndexes(const std::string& item, int seed = 0) const;
    void addPosition(const std::vector<int>& bits, const std::vector<uint64_t>& indexes, int seed = 0);
    bool isSet(uint64_t index) const;
    std::pair<std::vector<uint64_t>, std::vector<int>> returnPartialCollisionIndex(
        const std::vector<uint64_t>& indexes) const;
    uint64_t returnPosition(const std::string& kmer) const;

    std::size_t getSize() const;  
    const std::vector<bool>& getBitArray() const;

    static std::size_t calculateBitArraySize(std::size_t elementsToEncode, double falsePositiveRate);  // changed to std::size_t
    static int calculateOptimalHashNum(std::size_t elementsToEncode, std::size_t bitArraySize);  // changed to std::size_t
    static uint64_t binarySeqToDecimal(const std::vector<int>& bits);
};

