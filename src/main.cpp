#include <iostream>
#include "pthash.hpp"
#include "util.hpp"

int main() {
    using namespace pthash;

    std::vector<std::string> keys = {
        "apple", "banana", "cherry", "date", "elderberry",
        "fig", "grape", "honeydew", "kiwi", "lemon"
    };

    build_configuration config;
    config.seed = 42;
    config.lambda = 3;        
    config.alpha = 0.94;      
    config.verbose = true;   

    typedef single_phf<
        xxhash128,                   
        skew_bucketer,               
        compact_compact,             
        true,                        
        pthash_search_type::add_displacement  
    > pthash_type;

    pthash_type f;

    std::cout << "Building perfect hash function..." << std::endl;
    f.build_in_internal_memory(keys.begin(), keys.size(), config);

    std::cout << "Built PHF with "
        << f.num_bits() << " total bits ("
        << static_cast<double>(f.num_bits()) / f.num_keys()
        << " bits/key)" << std::endl;

    std::cout << "\nHash values:" << std::endl;
    for (const auto& key : keys) {
        auto hash = f(key);
        std::cout << key << " => " << hash << std::endl;
    }

    return 0;
}