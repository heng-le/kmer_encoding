# Fetch Catch2
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0  
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_executable(UnitTests
    bloomfilter_test.cpp
)
target_link_libraries(UnitTests
    PRIVATE
        Catch2::Catch2WithMain  
        CapstoneLibrary        
)
target_include_directories(UnitTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Move this to the end
catch_discover_tests(UnitTests)