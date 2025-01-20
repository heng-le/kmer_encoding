#pragma once
#include "BloomFilter.h"

class PartitionedBloomFilter : public BloomFilter {
private:
    std::size_t partitionSize;

    uint64_t generateHash(const std::string& item, int i, int seed = 0) const {
        uint8_t hash128[16];
        uint32_t modifiedSeed = seed + i;
        MurmurHash3_x64_128(item.c_str(), (int)item.size(), modifiedSeed, hash128);

        uint64_t hashValue = combine128to64(hash128);
        std::size_t partitionStart = i * partitionSize;
        return partitionStart + (hashValue % partitionSize);
    }

public:
    PartitionedBloomFilter(std::size_t elementsToEncode, double falsePositiveRate, int positionBits = 1)
        : BloomFilter(elementsToEncode, falsePositiveRate, positionBits) {
        partitionSize = getSize() / numHashCount;
    }

    std::vector<std::size_t> getPartitionSizes() const {
        std::vector<std::size_t> sizes;
        sizes.reserve(numHashCount);

        for (int i = 0; i < numHashCount; i++) {
            std::size_t start = i * partitionSize;
            std::size_t end = (i == numHashCount - 1) ? getSize() : (i + 1) * partitionSize;
            sizes.push_back(end - start);
        }
        return sizes;
    }

    int getPartitionIndex(std::size_t bitIndex) const {
        return static_cast<int>(bitIndex / partitionSize);
    }
};