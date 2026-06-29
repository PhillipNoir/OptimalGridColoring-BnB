#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include "GridColoring/BranchAndBound.hpp"
#include "GridColoring/GraphDefinition.hpp"

// =========================================================================
// SECTION 1: GOOGLE TESTS (Mathematical Correctness Validation)
// =========================================================================

// Helper function to verify if a solved grid is physically legal
bool IsGridValid(const Grid1D& grid) {
    int width = grid.getWidth();
    int height = grid.getHeight();
    
    for (int i = 0; i < width * height; ++i) {
        int color = grid.getColorByIndex(i);
        if (color == -1) return false; // Uncolored cell detected
        
        auto [x, y] = grid.getCoordinates(i);
        
        // Check Von Neumann neighborhood (Up, Down, Left, Right)
        if (y > 0 && grid.getColorByIndex(i - width) == color) return false;
        if (y < height - 1 && grid.getColorByIndex(i + width) == color) return false;
        if (x > 0 && grid.getColorByIndex(i - 1) == color) return false;
        if (x < width - 1 && grid.getColorByIndex(i + 1) == color) return false;
    }
    return true;
}

// Ensure the algorithm finds a legal solution for a 4x4 grid
TEST(BranchAndBoundTest, Resolves4x4Correctly) {
    SearchState finalState = SolveGrid(4, 4);
    EXPECT_TRUE(IsGridValid(finalState.bestGrid));
}

// Ensure the algorithm finds a legal solution for a 6x6 grid
TEST(BranchAndBoundTest, Resolves6x6Correctly) {
    SearchState finalState = SolveGrid(6, 6);
    EXPECT_TRUE(IsGridValid(finalState.bestGrid));
}

// =========================================================================
// SECTION 2: GOOGLE BENCHMARKS (CPU Stress and Complexity Analysis)
// =========================================================================

static void BM_GraphColoring(benchmark::State& state) {
    int size = state.range(0); // The N for an NxN grid
    
    for (auto _ : state) {
        // We only want to measure the algorithm, not the object instantiation
        state.PauseTiming();
        SearchState searchState(size, size);
        
        // Symmetry Reduction (as written in your API)
        searchState.grid.setColorByIndex(0, 1);
        searchState.colorCounts[1]++;
        searchState.uncoloredNodes--;
        
        state.ResumeTiming();
        
        // 1. Ignite the Greedy Upper Bound
        RunGreedyInitialSolution(searchState);
        
        // 2. Dive into the deep search
        ExecuteBranchAndBound(searchState);
        
        // Force the compiler to not optimize away our hard-earned result
        benchmark::DoNotOptimize(searchState.currentBestDiff);
    }
    
    // Register theoretical complexity for the reports
    state.SetComplexityN(state.range(0) * state.range(0));
}

// Register the required benchmarks (4x4, 6x6, 8x8, 10x10, 12x12)
BENCHMARK(BM_GraphColoring)
    ->Arg(4)
    ->Arg(6)
    ->Arg(8)
    ->Arg(10)
    ->Arg(12)
    ->Unit(benchmark::kMillisecond) 
    ->Complexity();

// =========================================================================
// SECTION 3: CUSTOM MAIN TO RUN BOTH FRAMEWORKS
// =========================================================================

int main(int argc, char** argv) {
    // 1. Initialize and run Google Test suite
    ::testing::InitGoogleTest(&argc, argv);
    int test_result = RUN_ALL_TESTS();

    // 2. If math is correct, proceed to stress the CPU
    if (test_result == 0) {
        ::benchmark::Initialize(&argc, argv);
        if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
        ::benchmark::RunSpecifiedBenchmarks();
        ::benchmark::Shutdown();
    } else {
        std::cerr << "[!] Tests failed. Aborting Benchmarks to prevent invalid data." << std::endl;
    }

    return test_result;
}