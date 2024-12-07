
cmake_minimum_required(VERSION 3.10)
project(cjson C)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find source files
file(GLOB COMMON_SOURCES "src/json.c")
file(GLOB EXAMPLE_SOURCES "src/*example.c")
file(GLOB TEST_SOURCES "src/*test*.c")

# Create example executable
add_executable(example ${COMMON_SOURCES} ${EXAMPLE_SOURCES})

# Create test executable
add_executable(test ${COMMON_SOURCES} ${TEST_SOURCES})