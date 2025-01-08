#include "BloomFilter.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include "../external/MurmurHash3/murmurhash3.h"


uint64_t BloomFilter::combine128to64(const uint8_t hash128[16]) {
    uint64_t low, high; 

    std::memcpy(&low, hash128, 8);
    std::memcpy(&high, hash128 + 8, 8);

    return low ^ high;
}

uint64_t BloomFilter::generateHash(const std::string& item, int i, int seed) const {
    uint8_t hash128[16];
    // Use the index i to modify the seed for different hash values
    uint32_t modifiedSeed = seed + i;
    MurmurHash3_x64_128(item.c_str(), static_cast<int>(item.length()), modifiedSeed, hash128);
    uint64_t hashValue = combine128to64(hash128);

    return hashValue % bitArraySize;
}

BloomFilter::BloomFilter(size_t elementsToEncode, double falsePositiveRate)
    : bitArraySize(calculateBitArraySize(elementsToEncode, falsePositiveRate)),
    numHashCount(calculateOptimalHashNum(elementsToEncode, bitArraySize)) {
    bitArray.resize(bitArraySize, false);
}

void BloomFilter::add(const std::string& item, int seed) {
    for (int i = 0; i < numHashCount; i++) {
        uint64_t hashIndex = generateHash(item, i, seed);
        bitArray[hashIndex] = true;
    }
}

bool BloomFilter::mightContain(const std::string& item, int seed) const {
    for (int i = 0; i < numHashCount; i++) {
        if (!bitArray[generateHash(item, i, seed)]) {
            return false;
        }
    }
    return true;
}

std::vector<uint64_t> BloomFilter::getHashIndexes(const std::string& item, int seed) const {
    std::vector<uint64_t> hashIndexes;
    hashIndexes.reserve(numHashCount);

    for (int i = 0; i < numHashCount; i++) {
        hashIndexes.push_back(generateHash(item, i, seed));
    }
    return hashIndexes;
}

void BloomFilter::addPosition(const std::vector<int>& bits, const std::vector<uint64_t>& indexes, int seed) {
    if (bits.size() != indexes.size()) {
        throw std::invalid_argument("Bits and indexes must have the same length");
    }

    for (size_t i = 0; i < bits.size(); i++) {
        if (bits[i] != 0) {
            bitArray[indexes[i]] = true;
        }
    }
}

bool BloomFilter::isSet(uint64_t index) const {
    return bitArray[index];
}

std::pair<std::vector<uint64_t>, std::vector<int>> BloomFilter::returnPartialCollisionIndex(
    const std::vector<uint64_t>& indexes) const {
    std::vector<uint64_t> indexArray;
    std::vector<int> iArray;

    for (int i = 0; i < indexes.size(); i++) {
        if (bitArray[indexes[i]]) {
            indexArray.push_back(indexes[i]);
            iArray.push_back(i);
        }
    }

    return { indexArray, iArray };
}

uint64_t BloomFilter::returnPosition(const std::string& kmer) const {
    auto indexes = getHashIndexes(kmer);
    std::vector<int> bitValues;
    bitValues.reserve(indexes.size());

    for (uint64_t index : indexes) {
        bitValues.push_back(isSet(index) ? 1 : 0);
    }

    return binarySeqToDecimal(bitValues);
}

size_t BloomFilter::getSize() const {
    return bitArraySize;
}

const std::vector<bool>& BloomFilter::getBitArray() const {
    return bitArray;
}

size_t BloomFilter::calculateBitArraySize(size_t elementsToEncode, double falsePositiveRate) {
    return std::ceil(-(elementsToEncode * std::log(falsePositiveRate)) / (std::log(2) * std::log(2)));
}

int BloomFilter::calculateOptimalHashNum(size_t elementsToEncode, size_t bitArraySize) {
    return std::max(1, static_cast<int>(std::round((bitArraySize / elementsToEncode) * std::log(2))));
}

uint64_t BloomFilter::binarySeqToDecimal(const std::vector<int>& bits) {
    uint64_t result = 0;
    size_t n = bits.size();

    for (size_t i = 0; i < n; i++) {
        if (bits[i] == 1) {
            result |= (1ULL << (n - i - 1));  
        }
    }
    return result;
}
