#pragma once

#include "BloomFilter.h" 

#include <string>

class BasicEncoding {
private:
    BloomFilter bloomFilter; 
    size_t kmerSize;        

public:
    BasicEncoding(size_t numElements, double falsePositiveRate, size_t k);

    void processFile(const std::string& filePath);
};
