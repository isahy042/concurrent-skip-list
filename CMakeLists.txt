cmake_minimum_required(VERSION 3.16)
project(SkipListProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files (exclude main.cpp)
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")
list(FILTER SRC_FILES EXCLUDE REGEX ".*main\\.cpp$")

# Add test files
set(TEST_DIR "${CMAKE_SOURCE_DIR}/test")
file(GLOB TEST_FILES "${TEST_DIR}/*.cpp")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Main executable target
add_executable(skip-list ${SRC_DIR}/main.cpp ${SRC_FILES})
target_include_directories(skip-list PRIVATE ${SRC_DIR})

# Test executable target
add_executable(skip-list-test ${TEST_FILES} ${SRC_FILES})
target_include_directories(skip-list-test PRIVATE ${SRC_DIR})

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")

# Link libatomic to both targets
target_link_libraries(skip-list PRIVATE atomic)
target_link_libraries(skip-list-test PRIVATE atomic)

# Custom "make test" target
add_custom_target(test ALL DEPENDS skip-list-test)
