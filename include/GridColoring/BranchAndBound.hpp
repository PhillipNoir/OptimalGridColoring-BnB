#pragma once

/**
 * @file BranchAndBound.hpp
 * @brief Core algorithm implementation combining DFS, heuristics, and mathematical pruning.
 * @author Sergio "Phillip Noir" Gonzalez
 * @date 2026-06-21
 * @version 1.0.0
 */

#include "GridColoring/GraphDefinition.hpp"
#include <vector>

/**
 * @brief Generates an initial valid coloring to establish a strong Upper Bound (UB).
 * * Uses a greedy approach to quickly find a valid solution and set the 
 * initial currentBestDiff in the state, allowing the algorithm to prune 
 * branches from the very beginning.
 * * @param state The search state to modify.
 */
void RunGreedyInitialSolution(SearchState& state);

/**
 * @brief Selects the next node to color using the Minimum Remaining Values (MRV) heuristic.
 * * Scans uncolored nodes and returns the index of the node that has the fewest
 * valid color options available. This ensures we tackle the most restrictive 
 * nodes first to fail fast and prune early.
 * * @param state The current search state.
 * @return The 1D index of the selected node.
 */
int SelectNextNodeMRV(const SearchState& state);

/**
 * @brief Sorts a list of available colors based on their current usage frequency.
 * * Colors that have been used the least are placed at the front of the vector
 * to encourage exploring highly balanced branches first, which improves the 
 * Upper Bound faster.
 * * @param availableColors The vector of valid colors for a specific node.
 * @param colorCounts The global tally of color usage.
 */
void SortColorsByBalance(std::vector<int>& availableColors, const std::vector<int>& colorCounts);

/**
 * @brief The core recursive Branch and Bound function.
 * * Explores the state space, applying Lower Bound (LB) pruning, the MRV heuristic, 
 * and backtracking to find the optimal balanced coloring.
 * * @param state The current search state, passed by reference to avoid deep copies.
 */
void ExecuteBranchAndBound(SearchState& state);

/**
 * @brief Public interface to solve a grid of given dimensions.
 * * Initializes the SearchState, applies symmetry reduction (fixing the first node),
 * runs the greedy initializer, and triggers the main Branch and Bound algorithm.
 * * @param width The width of the grid.
 * @param height The height of the grid.
 * @return The final SearchState containing the best optimal grid and difference.
 */
SearchState SolveGrid(int width, int height);