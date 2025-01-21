#include <iostream>
class PartitionedBloomFilter : public BloomFilter {
private:
    std::size_t partitionSize;

public:
    PartitionedBloomFilter(std::size_t elementsToEncode,
        double falsePositiveRate,
        int positionBits = 1)
        : BloomFilter(elementsToEncode, falsePositiveRate, positionBits)
    {
        if (numHashCount == 0) {
            throw std::invalid_argument("Number of hash functions cannot be zero");
        }
        partitionSize = getSize() / numHashCount;
        if (partitionSize == 0) {
            throw std::invalid_argument("Partition size cannot be zero");
        }
    }

    uint64_t generateHash(const std::string& item, int i, int seed = 0) const override {
        if (i < 0 || i >= numHashCount) {
            throw std::out_of_range("Hash function index out of range");
        }

        uint8_t hash128[16];
        uint32_t modifiedSeed = seed + i;
        MurmurHash3_x64_128(item.c_str(), static_cast<int>(item.size()),
            modifiedSeed, hash128);
        uint64_t hashValue = combine128to64(hash128);

        // Calculate partition bounds
        std::size_t start = i * partitionSize;
        std::size_t end = (i == numHashCount - 1)
            ? getSize()
            : (i + 1) * partitionSize;

        std::cout << "Partitioned index: " << start + (hashValue % (end - start)) << std::endl;

        return start + (hashValue % (end - start));
    }

    // Get statistics about each partition's utilization
    struct PartitionStats {
        std::size_t index;
        std::size_t size;
        double fillRatio;
    };

    std::vector<PartitionStats> getPartitionStats() const {
        std::vector<PartitionStats> stats;
        stats.reserve(numHashCount);

        for (int i = 0; i < numHashCount; i++) {
            std::size_t start = i * partitionSize;
            std::size_t end = (i == numHashCount - 1)
                ? getSize()
                : (i + 1) * partitionSize;

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

    // Get the sizes of all partitions
    std::vector<std::size_t> getPartitionSizes() const {
        std::vector<std::size_t> sizes;
        sizes.reserve(numHashCount);
        for (int i = 0; i < numHashCount; i++) {
            std::size_t start = i * partitionSize;
            std::size_t end = (i == numHashCount - 1)
                ? getSize()
                : (i + 1) * partitionSize;
            sizes.push_back(end - start);
        }
        return sizes;
    }

    // Given a global bit index, get which partition it belongs to
    int getPartitionIndex(std::size_t bitIndex) const {
        return static_cast<int>(bitIndex / partitionSize);
    }
};