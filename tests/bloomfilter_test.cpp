#include <catch2/catch_all.hpp>
#include "BloomFilter.h"
#include "PartitionedBloomFilter.h"
#include <string>
#include <vector>
#include <iostream>

// ------------------ Construction Tests ------------------ //
TEST_CASE("BloomFilter Construction", "[bloom]") {
    SECTION("Valid construction parameters") {
        // Now requires a positionBits parameter (e.g., 10 bits)
        REQUIRE_NOTHROW(BloomFilter(1000, 0.01, 10));
    }

    //SECTION("Verify initial size") {
    //    BloomFilter bf(1000, 0.01, 10);
    //    REQUIRE(bf.getSize() > 0);
    //}
}

TEST_CASE("PartitionedBloomFilter Construction", "[partitioned_bloom]") {
    SECTION("Valid construction parameters") {
        REQUIRE_NOTHROW(PartitionedBloomFilter(1000, 0.01, 10));
    }
}

// ------------------ Basic Operations ------------------ //
TEST_CASE("Basic Operations", "[bloom]") {
    // Create with 10 bits for position
    BloomFilter bf(1000, 0.01, 10);
    std::string testStr = "test_string";

    SECTION("Adding and checking elements") {
        // Initially should not contain element
        REQUIRE_FALSE(bf.mightContain(testStr));

        // Adding element with position=0 (or any number if you like)
        bf.add(testStr, 0);

        // Should now contain element
        REQUIRE(bf.mightContain(testStr));
    }

    SECTION("Multiple additions don't affect containment check") {
        bf.add(testStr, 0);
        bool firstCheck = bf.mightContain(testStr);
        bf.add(testStr, 0);
        bool secondCheck = bf.mightContain(testStr);
        REQUIRE(firstCheck);
        REQUIRE(firstCheck == secondCheck);
    }
}

TEST_CASE("Basic Partitioned Operations", "[par_bloom]") {
    // Create with 10 bits for position
    PartitionedBloomFilter bf(1000, 0.01, 10);
    std::string testStr = "test_string";

    SECTION("Adding and checking elements") {
        // Initially should not contain element
        REQUIRE_FALSE(bf.mightContain(testStr));

        // Adding element with position=0 (or any number if you like)
        bf.add(testStr, 0);

        // Should now contain element
        REQUIRE(bf.mightContain(testStr));
    }

    SECTION("Multiple additions don't affect containment check") {
        bf.add(testStr, 0);
        bool firstCheck = bf.mightContain(testStr);
        bf.add(testStr, 0);
        bool secondCheck = bf.mightContain(testStr);
        REQUIRE(firstCheck);
        REQUIRE(firstCheck == secondCheck);
    }
}

// ------------------ Hash Function Tests ------------------ //
TEST_CASE("Hash Function - Different Seeds Produce Different Hashes", "[bloom][hash]") {
    BloomFilter bf(1000, 0.01, 10);
    std::string testStr = "test_string";

    std::set<uint64_t> hashResults;

    SECTION("Hash values with different seeds should be distinct") {
        for (int seed = 1; seed <= 10; seed++) {
            uint64_t hashValue = bf.generateHash(testStr, seed);
            REQUIRE(hashResults.find(hashValue) == hashResults.end());  
            hashResults.insert(hashValue);
        }

        REQUIRE(hashResults.size() == 10);
    }
}

TEST_CASE("Hash Function - Same Seed Produces Same Hash", "[bloom][hash]") {
    BloomFilter bf(1000, 0.01, 10);
    std::string testStr = "test_string";

    SECTION("Consistent hash output with same seed") {
        int seed = 5;  

        uint64_t firstHash = bf.generateHash(testStr, seed);
        for (int i = 0; i < 10; i++) {
            uint64_t repeatedHash = bf.generateHash(testStr, seed);
            REQUIRE(firstHash == repeatedHash);  
        }
    }
}

TEST_CASE("Hash Function - Hash Distribution", "[bloom][hash]") {
    BloomFilter bf(1000, 0.01, 10);
    std::string testStr = "test_string";
    size_t filterSize = bf.getSize();

    SECTION("Hash outputs are well-distributed across the filter size") {
        std::vector<uint64_t> hashes;
        for (int seed = 1; seed <= 100; seed++) {
            uint64_t hashValue = bf.generateHash(testStr, seed);
            hashes.push_back(hashValue);
        }

        for (auto hash : hashes) {
            REQUIRE(hash < filterSize);
        }

        std::set<uint64_t> uniqueHashes(hashes.begin(), hashes.end());
        REQUIRE(uniqueHashes.size() > 90);  
    }
}


// ------------------ False Positive Rate ------------------ //
TEST_CASE("False Positive Rate", "[bloom]") {
    size_t numElements = 1000;
    double targetFPR = 0.01;

    BloomFilter bf(numElements, targetFPR, 10);

    SECTION("False positive rate is approximate") {
        for (size_t i = 0; i < numElements; i++) {
            bf.add("element" + std::to_string(i), 0);
        }

        size_t falsePositives = 0;
        size_t testElements = 10000;

        for (size_t i = numElements; i < numElements + testElements; i++) {
            if (bf.mightContain("test" + std::to_string(i))) {
                falsePositives++;
            }
        }

        double actualFPR = static_cast<double>(falsePositives) / testElements;
        REQUIRE(actualFPR < targetFPR * 1.2);
    }
}

// ------------------ Position Tracking ------------------ //
TEST_CASE("Position Tracking", "[bloom]") {
    BloomFilter bf(1000, 0.01, 10);
    std::string kmer = "ATCG";

    SECTION("Basic position storage and retrieval") {
        REQUIRE(bf.add(kmer, 42, 0));  
        REQUIRE(bf.mightContain(kmer));
        REQUIRE(bf.getPosition(kmer, 0) == 42);
    }

    SECTION("Position bit conflicts") {
        REQUIRE(bf.add(kmer, 42, 0));

        // Adding same k-mer with same position should succeed
        REQUIRE(bf.add(kmer, 42, 0));

        // Original position should remain unchanged
        REQUIRE(bf.getPosition(kmer, 0) == 42);
    }

    SECTION("Multiple k-mers with different positions") {
        REQUIRE(bf.add("ATCG", 100, 0));
        REQUIRE(bf.add("GCTA", 200, 0));

        REQUIRE(bf.getPosition("ATCG", 0) == 100);
        // intra-element collision occurs here
        REQUIRE_FALSE(bf.getPosition("GCTA", 0) == 200);
    }

    SECTION("Position bit boundary tests") {
        REQUIRE(bf.add("test1", 1023, 0));
        REQUIRE(bf.getPosition("test1", 0) == 1023);

        // Test position exceeding bit limit
        REQUIRE_FALSE(bf.add("test2", 1024, 0));
    }

}

TEST_CASE("Partitioned Position Tracking", "[par_bloom]") {
    PartitionedBloomFilter bf(1000, 0.01, 10);
    std::string kmer = "ATCG";

    SECTION("Basic position storage and retrieval") {
        REQUIRE(bf.add(kmer, 42, 0));
        REQUIRE(bf.mightContain(kmer));
        REQUIRE(bf.getPosition(kmer, 0) == 42);
    }

    SECTION("Position bit conflicts") {
        REQUIRE(bf.add(kmer, 42, 0));

        // Adding same k-mer with same position should succeed
        REQUIRE(bf.add(kmer, 42, 0));

        // Original position should remain unchanged
        REQUIRE(bf.getPosition(kmer, 0) == 42);
    }

    SECTION("Multiple k-mers with different positions") {
        REQUIRE(bf.add("ATCG", 100, 0));
        REQUIRE(bf.add("GCTA", 200, 0));

        REQUIRE(bf.getPosition("ATCG", 0) == 100);
        // intra-element collision occurs here
        REQUIRE(bf.getPosition("GCTA", 0) == 200);
    }

    SECTION("Position bit boundary tests") {
        REQUIRE(bf.add("test1", 1023, 0));
        REQUIRE(bf.getPosition("test1", 0) == 1023);

        // Test position exceeding bit limit
        REQUIRE_FALSE(bf.add("test2", 1024, 0));
    }
}

TEST_CASE("Edge Cases with Position Bits", "[bloom]") {
    SECTION("Zero position handling") {
        BloomFilter bf(1000, 0.01, 10);
        REQUIRE(bf.add("test", 0, 0));
        REQUIRE(bf.getPosition("test", 0) == 0);
    }

    SECTION("Empty string with position") {
        BloomFilter bf(1000, 0.01, 10);
        REQUIRE(bf.add("", 42, 0));
        REQUIRE(bf.getPosition("", 0) == 42);
    }

    SECTION("Large string with position") {
        BloomFilter bf(1000, 0.01, 10);
        std::string largeString(1000, 'a');
        REQUIRE(bf.add(largeString, 10, 0));
        REQUIRE(bf.getPosition(largeString, 0) == 10);
    }
}