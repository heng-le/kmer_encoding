#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include "predeterminedBloomFilter.h" 
#include "BloomFilter.h"     
#include <bit>
#include <cstdint>

std::vector<std::string> readFileLines(const std::string& filepath) {
    std::vector<std::string> lines;
    std::ifstream inFile(filepath);
    if (!inFile.is_open()) {
        throw std::runtime_error("Unable to open input file: " + filepath);
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    inFile.close();
    return lines;
}

// Helper function for writing lines to a file.
void writeFileLines(const std::string& filepath,
    const std::vector<std::string>& lines) {
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        throw std::runtime_error("Unable to open output file: " + filepath);
    }
    for (const auto& line : lines) {
        outFile << line << "\n";
    }
    outFile.close();
}

int numBits(uint64_t x) {
    if (x == 0) return 1;
    return 64 - std::countl_zero(x);
}



int main() {
    try {
        const std::string uniqueKmersPath = "D:/Research/Capstone/python_stuff/unique_37mers_1000.txt";

        const std::size_t elementsToEncode = 1000;
        const double falsePositiveRate = 0.001;

        const int numHash = numBits(elementsToEncode);

        const int positionBits = numBits(elementsToEncode);

        const int seed = 42;

        std::vector<PredeterminedHashBloomFilter> bloomFilters;
        bloomFilters.emplace_back(
            elementsToEncode,
            falsePositiveRate,
            numHash,
            positionBits
        );

        std::string currentRoundFile = uniqueKmersPath;
        bool collisionsExist = true;
        std::size_t round = 0;

        std::size_t totalElementsLeft = elementsToEncode;

        uint64_t currentPosition = 0;

        while (collisionsExist) {
            collisionsExist = false;
            std::cout << "\n--- Processing Round " << round
                << " (Elements left: " << totalElementsLeft << ") ---\n";

            const std::string collisionFilePath =
                "D:/Research/Capstone/capstone_code2/test_output/collisions_round_"
                + std::to_string(round) + ".txt";

            std::ifstream inFile(currentRoundFile);
            if (!inFile.is_open()) {
                throw std::runtime_error("Unable to open input file: " + currentRoundFile);
            }

            std::ofstream collisionFile(collisionFilePath);
            if (!collisionFile.is_open()) {
                throw std::runtime_error("Unable to open collision file: " + collisionFilePath);
            }

            std::string line;
            while (std::getline(inFile, line)) {
                if (line.empty()) {
                    continue; 
                }

                bool inserted = false;
                for (auto& bf : bloomFilters) {
                    if (bf.add(line, currentPosition, seed)) {
                        inserted = true;
                        totalElementsLeft--;
                        break;
                    }
                }

                if (!inserted) {
                    collisionFile << line << "\n";
                    collisionsExist = true;
                }
                currentPosition++;
            }

            inFile.close();
            collisionFile.close();

        
            if (collisionsExist) {
                currentRoundFile = collisionFilePath;
                bloomFilters.emplace_back(
                    elementsToEncode,
                    falsePositiveRate,
                    numHash,
                    positionBits
                );
                round++;
                currentPosition = 0;
            }
            else {
                std::cout << "No collisions found in this round. Done.\n";
            }
        }

        std::cout << "\nAll items processed successfully without further collisions.\n"
            << "Press ENTER to exit.\n";
        std::cin.get();

    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}