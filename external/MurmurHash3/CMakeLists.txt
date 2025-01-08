# external/MurmurHash3/CMakeLists.txt

project(MurmurHash3 LANGUAGES CXX)

# Create a static library from the MurmurHash3 sources.
add_library(MurmurHash3 STATIC
    murmurhash3.cpp
)

# Optionally set include dirs so the header can be found easily.
target_include_directories(MurmurHash3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)                    
