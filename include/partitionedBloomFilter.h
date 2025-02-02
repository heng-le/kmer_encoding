#pragma once
#include <iostream>
#include "bloomfilter.h"

class PartitionedBloomFilter : public BloomFilter {
protected:
    std::size_t partitionSize;

public:
    PartitionedBloomFilter(std::size_t elementsToEncode,
        double falsePositiveRate,
        int positionBits = 1,
        int numPartitions = 0)
        : BloomFilter(elementsToEncode, falsePositiveRate, positionBits),
        partitionSize(0)
    {
        if (numPartitions > 0) {
            numHashCount = numPartitions;
        }
        computePartitions();
    }

    virtual void computePartitions() {
        if (numHashCount == 0) {
            throw std::invalid_argument("[PartitionedBF] Number of hash functions cannot be zero");
        }
        if (getSize() < numHashCount) {
            throw std::invalid_argument("[PartitionedBF] Not enough bits for the requested partitions.");
        }

        partitionSize = getSize() / numHashCount;
        if (partitionSize == 0) {
            throw std::invalid_argument("[PartitionedBF] Partition size ended up zero. "
                "Increase total size or reduce number of partitions.");
        }
    }

    uint64_t generateHash(const std::string& item, int i, int seed = 0) const override {
        if (i < 0 || i >= numHashCount) {
            throw std::out_of_range("[PartitionedBF] Hash function index out of range");
        }

        uint8_t hash128[16];
        uint32_t modifiedSeed = seed + i;
        MurmurHash3_x64_128(item.c_str(), static_cast<int>(item.size()), modifiedSeed, hash128);
        uint64_t hashValue = combine128to64(hash128);

        std::size_t start = static_cast<std::size_t>(i) * partitionSize;
        std::size_t end = (i == numHashCount - 1)
            ? getSize()
            : (start + partitionSize);

        if (start >= end) {
            std::cerr << "[PartitionedBloomFilter] start >= end; invalid partition!\n";
            return 0;
        }

        std::size_t range = end - start;
        std::size_t offset = hashValue % range;

        uint64_t partitionedIndex = start + offset;
        //std::cout << "Partitioned index: " << partitionedIndex << std::endl;
        return partitionedIndex;
    }

    struct PartitionStats {
        std::size_t index;
        std::size_t size;
        double fillRatio;
    };

    std::vector<PartitionStats> getPartitionStats() const {
        std::vector<PartitionStats> stats;
        stats.reserve(numHashCount);

        for (int i = 0; i < numHashCount; i++) {
            std::size_t start = static_cast<std::size_t>(i) * partitionSize;
            std::size_t end = (i == numHashCount - 1)
                ? getSize()
                : (start + partitionSize);

            std::size_t setBits = 0;
            for (std::size_t j = start; j < end; j++) {
                if (presenceBitset[j]) setBits++;
            }

            stats.push_back({
                static_cast<std::size_t>(i),
                end - start,
                static_cast<double>(setBits) / (end - start)
                });
        }
        return stats;
    }

    std::vector<std::size_t> getPartitionSizes() const {
        std::vector<std::size_t> sizes;
        sizes.reserve(numHashCount);

        for (int i = 0; i < numHashCount; i++) {
            std::size_t start = static_cast<std::size_t>(i) * partitionSize;
            std::size_t end = (i == numHashCount - 1)
                ? getSize()
                : (start + partitionSize);
            sizes.push_back(end - start);
        }
        return sizes;
    }


    int getPartitionIndex(std::size_t bitIndex) const {
        return static_cast<int>(bitIndex / partitionSize);
    }
};
