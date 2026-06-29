# OptimalGridColoring-BnB

![C++](https://img.shields.io/badge/C++-23-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.10+-green.svg)
![Linux](https://img.shields.io/badge/OS-Linux-orange.svg)
![Google Test](https://img.shields.io/badge/Google_Test-1.12.1-orange.svg)
![Google Benchmark](https://img.shields.io/badge/Google_Benchmark-v1.8.3-red.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

A highly optimized C++ implementation of the Branch and Bound algorithm to solve the balanced coloring problem on von Neumann grids. Features custom heuristics, mathematical pruning, and comprehensive benchmarking.

## Overview

This project implements a high-performance, deterministic engine to solve the Equitable Graph Coloring Problem applied to 2D regular grids. The goal is to color an $N \times M$ grid using a maximum of 4 colors such that no two adjacent nodes share the same color (Von Neumann neighborhood), while simultaneously minimizing the frequency difference between the most used and least used colors.While this specific variant of graph coloring is theoretically classified as NP-Hard, this engine utilizes advanced heuristics and state-space pruning to collapse the exponential time complexity into near-linear observed execution times for geometrically regular grids.

## Theoretical Framework & Algorithm Complexity

### The NP-Hardness of Equilibrium

Standard graph coloring on a bipartite grid is trivial ($O(N)$ using two alternating colors). However, enforcing the Equilibrium Constraint (requiring the uniform distribution of 4 colors) destroys the bipartite symmetry, forcing the algorithm into an exponential search space of $O(4^N)$.

### The Engineering Triumph: Defeating $O(4^N)$

Despite the theoretical $O(4^N)$ worst-case complexity, this engine achieves microsecond execution times even on dense grids (e.g., $12 \times 12$ / 144 nodes). This is achieved by exploiting the specific geometric properties of the grid:

1. **Bounded Degree**: Grid nodes have a maximum degree of 4. Localizing the constraint scope prevents chaotic ripple effects common in random graphs.
2. **The "Zero-Search" Phenomenon**: By pairing a highly effective Greedy Initializer with strict Mathematical Pruning, the algorithm frequently achieves the optimal solution at the root node. The Branch and Bound engine evaluates that the theoretical lower bound matches the current best solution, instantly pruning the entire $O(4^N)$ tree without generating a single child node.

## Algorithms Implemented

1. **Greedy Initializer (Upper Bound Generator)**

    Before the deep search begins, a parallel simulation state runs a greedy algorithm.
    - It sorts all nodes by their degree in descending order (most restrictive first).
    - It sequentially assigns the valid color that has been used the least so far.
    - Purpose: This establishes a highly optimized initial "High Score" (Upper Bound). Without this, the deep search would explore blindly.

2. **Minimum Remaining Values (MRV) Heuristic (Node Selection)**

    When selecting the next node to process, the algorithm does not read the grid linearly. Instead, it scans for the node with the fewest valid color options available.
    - This prioritizes the most constrained nodes, forcing early decisions and exposing dead-ends faster.
    - Purpose: "Fail Fast." By tackling the most constrained nodes immediately, the algorithm forces dead-end branches to collapse early, saving millions of useless CPU cycles.

3. **Branch and Bound (DFS Core)**

    The core recursive engine navigates the state space using Depth-First Search with backtracking.
    - **Symmetry Reduction**: The algorithm forcefully pins the top-left coordinate (0,0) to Color 1, instantly dividing the global search space by 4.
    - **Balance Heuristic**: Valid colors are sorted in ascending order of their usage frequency, prioritizing balanced universes.
    - **Mathematical Pruning**: At every node, an $O(1)$ calculation predicts the best possible future outcome. If this Lower Bound prediction is worse than the Greedy Upper Bound, the entire temporal branch is permanently destroyed.

## Architecture & Technologies

This project is structured using enterprise-level C++ conventions, separating core logic from testing suites to ensure a lightweight production binary.

- **C++23**: Leveraging modern language features for type safety and performance.
- **CMake**: Advanced build system configuration for cross-platform compatibility.
- **GoogleTest & Google Benchmark**: Industry-standard frameworks for unit testing and performance benchmarking, integrated via `FetchContent` for zero-configuration dependency management.

## Build & Run Instructions

This project uses an out-of-source build system. It is recommended to build this project using the provided build script. Ensure you have the necessary build dependencies installed (g++, cmake, and make).

1. Clone the repository and navigate to the root directory:

```bash
  git clone <repository-url>
  cd OptimalGridColoring-BnB
```

2. Create a build directory and navigate into it:

```bash
  mkdir build
  cd build
```

3. Generate the build files using CMake:

```bash
  cmake ..
```

4. Build the project:

```bash
  make -j$(nproc)
```

5. Run the tests and benchmarks:

```bash
  ./PerformanceTests
```

## Benchmarking Analysis

When running the PerformanceTests executable, Google Benchmark will output the execution times for varying grid sizes ($4\times4$, $6\times6$, $8\times8$, $10\times10$, $12\times12$).Due to the effectiveness of the heuristics mentioned in the theoretical framework, you will observe execution times in the fractions of a millisecond.In sizes where the Greedy Initializer finds the perfect global optimum (Difference = 0), the Branch and Bound engine achieves a 100% pruning rate, resulting in $O(1)$ search time.In edge cases (such as specific $8\times8$ configurations) where the Greedy solution is imperfect, the CPU dynamically scales to perform a localized exhaustive search to bridge the gap to global optimality, validating the reliability of the safety fallback mechanisms.

## Author

Sergio "Phillip Noir" Gonzalez Cruz -- Computer Engineering Student and video game enthusiast. This project was completed as a requirement for the Analysis and Design of Algorithms course at Universidad Autonoma Metropolitana (UAM Azcapotzalco). If it has helped you, please consider giving it a star! or buying me a coffee:

https://coff.ee/phillipnoir