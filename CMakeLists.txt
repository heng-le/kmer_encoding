﻿cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Capstone_v2")

message(STATUS "PTHASH includes: ${CMAKE_SOURCE_DIR}/external/pthash/include")
enable_testing()
set(CMAKE_BUILD_TYPE Release)

# Include sub-projects.
add_subdirectory("tests")
add_subdirectory("external/MurmurHash3")
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable Google Benchmark tests" FORCE)
add_subdirectory("external/benchmark")
add_subdirectory("external/pthash")
add_subdirectory("src")





