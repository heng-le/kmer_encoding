#pragma once

#include "BloomFilter.h" 

#include <string>

class BasicEncoding {
private:
    BloomFilter bloomFilter; 
    size_t kmerSize;        

public:
    // Constructor: initializes the BloomFilter and k-mer size
    BasicEncoding(size_t numElements, double falsePositiveRate, size_t k);

    // Process the input file containing the genome
    void processFile(const std::string& filePath);
};
