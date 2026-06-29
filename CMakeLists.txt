cmake_minimum_required(VERSION 3.14)
project(OptimalGridColoring_BnB)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Aggressive optimizations for the Branch and Bound algorithm (Critical for CPU performance)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -DNDEBUG")

# --- Include Directories ---
# This maps the include folder so you can use #include "GridColoring/..." anywhere
include_directories(include)

# --- Core Logic Library ---
# We bundle your source files into a static library for cleaner linking
add_library(GridColoringLib STATIC
    src/BoundCalculations.cpp
    src/BranchAndBound.cpp
    src/GraphDefinition.cpp
)

# --- Google Test Integration ---
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# --- Google Benchmark Integration ---
FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.3
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark tests" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

# --- The Monolithic Test Executable ---
add_executable(PerformanceTests tests/benchmarks_and_tests.cpp)

# Link your core library, GTest, GBenchmark, and POSIX threads (required for Linux)
target_link_libraries(PerformanceTests 
    GridColoringLib
    gtest_main 
    benchmark::benchmark 
    pthread
)