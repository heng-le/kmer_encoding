#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "BloomFilter.h"
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

// ------------------ Hash Function Tests ------------------ //
TEST_CASE("Hash Functions", "[bloom]") {
    BloomFilter bf(1000, 0.01, 10);
    std::string testStr = "test_string";


}

// ------------------ False Positive Rate ------------------ //
TEST_CASE("False Positive Rate", "[bloom]") {
    size_t numElements = 1000;
    double targetFPR = 0.01;

    // Now passing positionBits=10 arbitrarily
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
        BloomFilter bf(1000, 0.01, 10);
        REQUIRE(bf.add("ATCG", 100, 0));
        REQUIRE(bf.add("GCTA", 96, 0));

        REQUIRE(bf.getPosition("ATCG", 0) == 100);
        REQUIRE(bf.getPosition("GCTA", 0) == 96);
    }

    SECTION("Position bit boundary tests") {
        // Test maximum allowed position (2^10 - 1 = 1023 for 10 bits)
        BloomFilter bf(1000, 0.01, 10);
        REQUIRE(bf.add("test1", 1023, 0));
        REQUIRE(bf.getPosition("test1", 0) == 1023);

        // Test position exceeding bit limit
        REQUIRE_FALSE(bf.add("test2", 1024, 0));
    }

    //SECTION("Overlapping position bits") {
    //    std::string item1 = "AAAA";
    //    std::string item2 = "CCCC";

    //    // Add first item
    //    REQUIRE(bf.add(item1, 42, 0));

    //    bool addResult = bf.add(item2, 42, 0);
    //    if (!addResult) {
    //        // If add failed, verify first item's position is unchanged
    //        REQUIRE(bf.getPosition(item1, 0) == 42);
    //    }
    //}
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