#pragma once
#include "PartitionedBloomFilter.h"
#include "bloomfilter.h"

class PredeterminedHashBloomFilter : public PartitionedBloomFilter {
private:
    const int predefinedHashCount;

    std::size_t calculateOptimalSize(std::size_t elementsToEncode,
        double falsePositiveRate,
        int hashCount) const {
        double denominator = std::log(1.0 - std::pow(falsePositiveRate, 1.0 / hashCount));
        if (denominator == 0) {
            throw std::invalid_argument("Invalid FPR or hashCount -> zero denominator");
        }
        return static_cast<std::size_t>(std::ceil(-((hashCount * elementsToEncode) / denominator)));
    }

public:
    PredeterminedHashBloomFilter(std::size_t elementsToEncode,
        double falsePositiveRate,
        int numHash,
        int positionBits = 1)
        : PartitionedBloomFilter(elementsToEncode, falsePositiveRate, positionBits),
        predefinedHashCount(numHash)
    {
        if (numHash <= 0) {
            throw std::invalid_argument("Number of hash functions must be positive");
        }

        bitArraySize = calculateOptimalSize(elementsToEncode, falsePositiveRate, numHash);
        if (bitArraySize == 0) {
            throw std::invalid_argument("Calculated bit array size cannot be zero");
        }

        presenceBitset.resize(bitArraySize, false);

        numHashCount = numHash;

        chunkCount = (positionBits + numHashCount - 1) / numHashCount;
        positionBitsets.resize(chunkCount);
        for (size_t b = 0; b < chunkCount; b++) {
            positionBitsets[b].resize(bitArraySize, false);
        }

        computePartitions();
    }

    uint64_t generateHash(const std::string& item, int i, int seed = 0) const override {
        return PartitionedBloomFilter::generateHash(item, i, seed);
    }

    std::size_t getSize() const override {
        return bitArraySize;
    }
};
