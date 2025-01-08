# Fetch Catch2
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0       
)
FetchContent_MakeAvailable(Catch2)

# Add the test executable
add_executable(UnitTests  "bloomfilter_test.cpp")

# Link the library that no longer includes main()
target_link_libraries(UnitTests PRIVATE Catch2::Catch2WithMain CapstoneLibrary)

include(CTest)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

catch_discover_tests(UnitTests)
