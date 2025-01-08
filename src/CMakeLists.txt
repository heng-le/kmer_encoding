# 1) Add a library from the .cpp files that do not contain main()
add_library(CapstoneLibrary STATIC
    bloomfilter.cpp
)

target_link_libraries(CapstoneLibrary
    PRIVATE
        MurmurHash3
)

# 2) The main application target: includes the file with your main()
add_executable(Capstone_v2
    main.cpp
)

# 3) Link the main app with the library
target_link_libraries(Capstone_v2
    PRIVATE
        CapstoneLibrary
)

# 4) Set the C++ standard if desired
if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Capstone_v2 PROPERTY CXX_STANDARD 20)
endif()
