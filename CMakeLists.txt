cmake_minimum_required(VERSION 3.10)
project(FastHashMap)

# Add header files
set(HEADERS
    inc/fast-hashmap.hpp
)

# Add the executable target for the main program
add_executable(${PROJECT_NAME}
    src/fast-hashmap.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE inc)

# Set C++ standard to C++20
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)


# Add tests executable target
add_executable(tests
    test/tests.cpp
)

# Include directories for tests
target_include_directories(tests PRIVATE inc)

# Register tests
enable_testing()
add_test(NAME FastHashMapTests COMMAND tests)
