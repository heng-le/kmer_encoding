#include "BloomFilter.h"
#include "../external/MurmurHash3/murmurhash3.h"
#include <cmath>
#include <cstring>
#include <algorithm>

// ------------------ Hashing Functions ------------------ //
uint64_t BloomFilter::combine128to64(const uint8_t hash128[16]) {
    uint64_t low, high;
    std::memcpy(&low, hash128, 8);
    std::memcpy(&high, hash128 + 8, 8);
    return low ^ high;
}

uint64_t BloomFilter::generateHash(const std::string& item, int i, int seed) const {
    uint8_t hash128[16];
    uint32_t modifiedSeed = seed + i;
    MurmurHash3_x64_128(item.c_str(), (int)item.size(), modifiedSeed, hash128);

    uint64_t hashValue = combine128to64(hash128);
    return hashValue % bitArraySize;
}

// ------------------ Constructor ------------------ //
BloomFilter::BloomFilter(std::size_t elementsToEncode,
    double falsePositiveRate,
    int positionBits)
    : positionBits(positionBits)

   
{
    // calculate optimal size of bloom filter
    bitArraySize = static_cast<std::size_t>(std::ceil(
        -(elementsToEncode * std::log(falsePositiveRate)) / (std::log(2) * std::log(2))
    ));

    // resizing presence bitset according to calculations
    presenceBitset.resize(bitArraySize, false);

    // calculate optimal number of hash functions
    numHashCount = std::max(1, static_cast<int>(std::round(
        (bitArraySize / static_cast<double>(elementsToEncode)) * std::log(2)
    )));

    // calculate how many coupled bit arrays needed for position encoding
    chunkCount = (positionBits + numHashCount - 1) / numHashCount;

    positionBitsets.resize(chunkCount);
    for (size_t b = 0; b < chunkCount; b++) {
        positionBitsets[b].resize(bitArraySize, false);
    }
}

// ------------------ Presence Bloom Filter  ------------------ //
void BloomFilter::addPresence(const std::string& item, int seed) {
    for (int i = 0; i < numHashCount; i++) {
        uint64_t index = generateHash(item, i, seed);
        presenceBitset[index] = true;
    }
}

bool BloomFilter::mightContain(const std::string& item, int seed) const {
    for (int i = 0; i < numHashCount; i++) {
        uint64_t index = generateHash(item, i, seed);
        if (!presenceBitset[index]) {
            return false;
        }
    }
    return true;
}

// ------------------ Position Bloom Filters ------------------ //
void BloomFilter::addPosition(const std::string& item, uint64_t position, int seed) {
    for (int i = 0; i < numHashCount; i++) {
        uint64_t index = generateHash(item, i, seed);
    }
    
    std::vector<bool> bits = encodePosition(position);

    std::vector<uint64_t> hashIndexes(numHashCount);
    for (int i = 0; i < numHashCount; i++) {
        hashIndexes[i] = generateHash(item, i, seed);
    }

    for (size_t b = 0; b < chunkCount; b++) {
        for (int i = 0; i < numHashCount; i++) {
            size_t bitIndex = b * numHashCount + i;
            if (bitIndex >= bits.size()) break;

            if (bits[bitIndex]) {
                positionBitsets[b][hashIndexes[i]] = true;
            }
        }
    }
}

uint64_t BloomFilter::getPosition(const std::string& item, int seed) const {
    if (!mightContain(item, seed)) {
        return 0ULL;  
    }

    std::vector<uint64_t> hashIndexes(numHashCount);
    for (int i = 0; i < numHashCount; i++) {
        hashIndexes[i] = generateHash(item, i, seed);
    }

    std::vector<bool> bits(positionBits, false);

    for (size_t b = 0; b < chunkCount; b++) {
        for (int i = 0; i < numHashCount; i++) {
            size_t bitIndex = b * numHashCount + i;
            if (bitIndex >= positionBits) break;

            if (positionBitsets[b][hashIndexes[i]]) {
                bits[bitIndex] = true;
            }
        }
    }
    uint64_t reconstructed = 0ULL;
    for (size_t i = 0; i < bits.size(); i++) {
        if (bits[i]) {
            reconstructed |= (1ULL << i);
        }
    }

    return reconstructed;
}

// ------------------ Combined Encoding ------------------ //
void BloomFilter::add(const std::string& item, uint64_t position, int seed) {
    
    std::vector<uint64_t> hashIndexes(numHashCount);
    for (int i = 0; i < numHashCount; i++) {
        hashIndexes[i] = generateHash(item, i, seed);
    }

    // 2) Mark presence bits (the normal Bloom filter part)
    for (uint64_t hIndex : hashIndexes) {
        presenceBitset[hIndex] = true;
    }

    // 3) Encode the position into bits
    std::vector<bool> bits = encodePosition(position);

    // 4) Mark position bits across the chunked bitsets
    for (size_t b = 0; b < chunkCount; b++) {
        for (int i = 0; i < numHashCount; i++) {
            size_t bitIndex = b * numHashCount + i;
            if (bitIndex >= bits.size()) break;  

            if (bits[bitIndex]) {
                positionBitsets[b][hashIndexes[i]] = true;
            }
        }
    }
}


// Helper to convert a position into binary bits with padding
std::vector<bool> BloomFilter::encodePosition(uint64_t position) const {
    std::vector<bool> bits(positionBits, false); 

    for (size_t i = 0; i < positionBits; i++) {
        bits[positionBits - i - 1] = (position >> i) & 1ULL;  
    }

    return bits;
}
