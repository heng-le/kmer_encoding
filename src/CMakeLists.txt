add_library(CapstoneLibrary STATIC
    bloomfilter.cpp)

target_link_libraries(CapstoneLibrary
    PRIVATE
        MurmurHash3
)


target_include_directories(CapstoneLibrary
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(Capstone_v2
main.cpp 
 )

target_link_libraries(Capstone_v2
    PRIVATE
        CapstoneLibrary
)

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Capstone_v2 PROPERTY CXX_STANDARD 20)
endif()


