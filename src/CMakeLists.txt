# Define a static library for CapstoneLibrary
add_library(CapstoneLibrary STATIC
    bloomfilter.cpp
)

# Link required dependencies
target_link_libraries(CapstoneLibrary
    PRIVATE
        MurmurHash3
    PUBLIC
        PTHASH
)

target_include_directories(CapstoneLibrary
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(Capstone_v2 main.cpp)
add_executable(partitioned partitionedEncoding.cpp)
add_executable(predetermined predeterminedEncoding.cpp)
add_executable(hashmap hashMapTest.cpp)
add_executable(mphf mphEncoding.cpp)



target_link_libraries(Capstone_v2
    PRIVATE
        CapstoneLibrary
)

target_link_libraries(mphf
    PRIVATE
        CapstoneLibrary
)

target_link_libraries(hashmap
    PRIVATE
        CapstoneLibrary
)

target_link_libraries(partitioned
    PRIVATE
        CapstoneLibrary
)

target_link_libraries(predetermined
    PRIVATE
        CapstoneLibrary
)

if(CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET Capstone_v2 PROPERTY CXX_STANDARD 20)
endif()
