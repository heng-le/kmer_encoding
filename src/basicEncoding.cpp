#include "BloomFilter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class BasicEncoding {
private:
    BloomFilter bloomFilter;
    size_t kmerSize;

public:
    BasicEncoding(size_t numElements, double falsePositiveRate, size_t k)
        : bloomFilter(numElements, falsePositiveRate), kmerSize(k) {}

    void processFile(const std::string& filePath) {
        std::ifstream inputFile(filePath);
        if (!inputFile.is_open()) {
            std::cerr << "Error: Unable to open file " << filePath << std::endl;
            return;
        }

        const size_t bufferSize = kmerSize * 30; 
        std::string carryOver;
        int duplicateCount = 0;

        while (inputFile.good()) {
            
            std::string chunk(bufferSize, '\0');
            inputFile.read(&chunk[0], bufferSize); 
            size_t bytesRead = inputFile.gcount(); 
            chunk.resize(bytesRead);

            std::string buffer = carryOver + chunk;
          
            for (size_t i = 0; i + kmerSize <= buffer.size(); ++i) {
                std::string kmer = buffer.substr(i, kmerSize);

                if (bloomFilter.mightContain(kmer)) {
                    duplicateCount++;
                    std::cout << "Duplicate found: " << kmer << std::endl;
                }
                else {
                    bloomFilter.add(kmer);
                }
            }

            if (buffer.size() >= kmerSize) {
                carryOver = buffer.substr(buffer.size() - kmerSize + 1);
            }
            else {
                carryOver = buffer; 
            }
        }

        inputFile.close();

        std::cout << "Processing complete.\n";
        std::cout << "Total duplicates detected: " << duplicateCount << std::endl;
    }

};
