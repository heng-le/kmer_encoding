#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../src/BloomFilter.h"
#include <string>
#include <vector>
#include <iostream>

TEST_CASE("BloomFilter Construction", "[bloom]") {
    SECTION("Valid construction parameters") {
        REQUIRE_NOTHROW(BloomFilter(1000, 0.01));
    }

    SECTION("Verify initial size") {
        BloomFilter bf(1000, 0.01);
        REQUIRE(bf.getSize() > 0);
    }
}

TEST_CASE("Basic Operations", "[bloom]") {
    BloomFilter bf(1000, 0.01);
    std::string testStr = "test_string";

    SECTION("Adding and checking elements") {
        // Initially should not contain element
        REQUIRE_FALSE(bf.mightContain(testStr));

        // Adding element
        bf.add(testStr);

        // Should now contain element
        REQUIRE(bf.mightContain(testStr));
    }

    SECTION("Multiple additions don't affect containment check") {
        bf.add(testStr);
        bool firstCheck = bf.mightContain(testStr);
        bf.add(testStr);
        bool secondCheck = bf.mightContain(testStr);
        REQUIRE(firstCheck);
        REQUIRE(firstCheck == secondCheck);
    }

}

TEST_CASE("Hash Functions", "[bloom]") {
    BloomFilter bf(1000, 0.01);
    std::string testStr = "test_string";

    SECTION("Hash indexes are within bounds") {
        auto indexes = bf.getHashIndexes(testStr);
        for (const auto& index : indexes) {
            REQUIRE(index < bf.getSize());
        }
    }

    SECTION("Same string produces same hash indexes") {
        auto indexes1 = bf.getHashIndexes(testStr);
        auto indexes2 = bf.getHashIndexes(testStr);
        REQUIRE(indexes1 == indexes2);
    }

    SECTION("Different strings likely produce different indexes") {
        auto indexes1 = bf.getHashIndexes("string1");
        auto indexes2 = bf.getHashIndexes("string2");
        REQUIRE(indexes1 != indexes2);
    }
}

TEST_CASE("False Positive Rate", "[bloom]") {
    size_t numElements = 1000;
    double targetFPR = 0.01;
    BloomFilter bf(numElements, targetFPR);

    SECTION("False positive rate is approximate") {
        // adding numElements different strings
        for (size_t i = 0; i < numElements; i++) {
            bf.add("element" + std::to_string(i));
        }

        // Test false positive rate with non-inserted elements
        size_t falsePositives = 0;
        size_t testElements = 10000;

        for (size_t i = numElements; i < numElements + testElements; i++) {
            if (bf.mightContain("test" + std::to_string(i))) {
                falsePositives++;
            }
        }

        double actualFPR = static_cast<double>(falsePositives) / testElements;
        // Allow for some variance in the false positive rate - roughly 1.2x
        REQUIRE(actualFPR < targetFPR * 1.2);  
    }
}

TEST_CASE("Bit Operations", "[bloom]") {
    BloomFilter bf(1000, 0.01);

    SECTION("Set and check bits") {
        std::vector<int> bits = { 1, 0, 1 };
        std::vector<uint64_t> indexes = { 0, 1, 2 };

        bf.addPosition(bits, indexes);

        REQUIRE(bf.isSet(0));
        REQUIRE_FALSE(bf.isSet(1));
        REQUIRE(bf.isSet(2));
    }

    //SECTION("Partial collision detection") {
    //    std::vector<int> bits = { 1, 0, 1 };
    //    std::vector<uint64_t> indexes = { 0, 1, 2 };
    //    bf.addPosition(bits, indexes);

    //    std::vector<uint64_t> testIndexes = { 0, 1, 2, 3 };
    //    auto [collisionIndexes, collisionPositions] = bf.returnPartialCollisionIndex(testIndexes);

    //    REQUIRE(collisionIndexes.size() == 2);  // Should find two collisions (0 and 2)
    //    REQUIRE(collisionPositions.size() == 2);
    //}
}

TEST_CASE("Edge Cases", "[bloom]") {
    SECTION("Empty string handling") {
        BloomFilter bf(1000, 0.01);
        REQUIRE_NOTHROW(bf.add(""));
        REQUIRE(bf.mightContain(""));
    }

    SECTION("Invalid addPosition parameters") {
        BloomFilter bf(1000, 0.01);
        std::vector<int> bits = { 1, 0 };
        std::vector<uint64_t> indexes = { 0 };  
        REQUIRE_THROWS(bf.addPosition(bits, indexes));
    }

    SECTION("Very large strings") {
        BloomFilter bf(1000, 0.01);
        std::string largeString(1000000, 'a'); 
        REQUIRE_NOTHROW(bf.add(largeString));
        REQUIRE(bf.mightContain(largeString));
    }
}

//TEST_CASE("Position Tracking", "[bloom]") {
//    BloomFilter bf(1000, 0.01);
//    std::string kmer = "ATCG";
//
//    SECTION("Position consistency") {
//        uint64_t pos1 = bf.returnPosition(kmer);
//        uint64_t pos2 = bf.returnPosition(kmer);
//        REQUIRE(pos1 == pos2);
//    }
//
//    SECTION("Different kmers have different positions") {
//        uint64_t pos1 = bf.returnPosition("ATCG");
//        uint64_t pos2 = bf.returnPosition("GCTA");
//        std::cout << pos1 << std::endl;
//        REQUIRE(pos1 != pos2);
//    }
//}

TEST_CASE("SeqUtils Test", "[bloom]") {
    SECTION("Binary Seq to Decimal") {
        BloomFilter bf(1000, 0.01);
        uint64_t decimal = 12;
        std::vector<int> testVector;
        testVector.push_back(1);
        testVector.push_back(1);
        testVector.push_back(0);
        testVector.push_back(0);
        REQUIRE(bf.binarySeqToDecimal(testVector) == decimal);
    }

}