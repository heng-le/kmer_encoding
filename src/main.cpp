#include "../external/MurmurHash3/murmurhash3.h"
#include <iostream>
#include <string>
#include <cstdint>

int main() {
    const char* key = "Hello, MurmurHash3!";
    uint32_t seed = 42;  


    // Example: 128-bit MurmurHash3
    uint8_t hash128[16];  
    MurmurHash3_x64_128(key, static_cast<int>(strlen(key)), seed, hash128);

    std::cout << "128-bit MurmurHash3: ";
    for (int i = 0; i < 16; i++) {
        printf("%02x", hash128[i]);
    }
    std::cout << std::endl;

    return 0;
}
