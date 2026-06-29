#pragma once

/**
 * @file BoundCalculations.hpp
 * @brief Mathematical heuristics and bounding functions for the Branch and Bound algorithm.
 * @author Sergio "Phillip Noir" Gonzalez
 * @date 2026-06-21
 */

#include <vector>
#include <algorithm>

/**
 * @brief Calculates the mathematical Lower Bound (LB) for the current state.
 * * This function relaxes the adjacency constraint to predict the best possible
 * balance we could achieve with the remaining nodes.
 * * @param colorCounts Vector with the frequency of each color currently used.
 * @param totalNodes The total size of the grid (width * height).
 * @return The absolute minimum difference (LB) achievable from this state.
 */
int CalculateLowerBound(const std::vector<int>& colorCounts, int totalNodes, int uncoloredNodes);

/**
 * @brief Utility function to get the minimum frequency among active colors.
 */
inline int GetMinCount(const std::vector<int>& colorCounts) {
    // We ignore index 0, checking only indices 1, 2, 3, and 4
    return *std::min_element(colorCounts.begin() + 1, colorCounts.end());
}

/**
 * @brief Utility function to get the maximum frequency among active colors.
 */
inline int GetMaxCount(const std::vector<int>& colorCounts) {
    return *std::max_element(colorCounts.begin() + 1, colorCounts.end());
}