#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cctype>
#include <limits>

// to trim whitespace
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

int main() {
    const std::string uniqueKmersPath = "/mnt/d/Research/Capstone_v2/test_files/unique_37mers_1000.txt";
    std::unordered_map<std::string, int> kmerMap;
    int position = 0;

    std::ifstream file(uniqueKmersPath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << uniqueKmersPath << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        kmerMap[trimmed] = position++;
    }

    const std::string testKmer = "TTTCTCTCAACTCAACAAAATGATTGGGCGACACGGG";
    auto it = kmerMap.find(testKmer);
    if (it != kmerMap.end()) {
        std::cout << "The k-mer '" << testKmer << "' is found at position " << it->second << std::endl;
    }
    else {
        std::cout << "The k-mer '" << testKmer << "' is not found in the map." << std::endl;
    }

    std::cout << "K-mers tracked in unordered_map. Press ENTER to exit." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return 0;

} 