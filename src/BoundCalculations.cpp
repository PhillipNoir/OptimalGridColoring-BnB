/**
 * @file BoundCalculations.cpp
 * @brief Implementation of the bounding math using pure integer arithmetic.
 * @author Sergio "Phillip Noir" Gonzalez
 * @date 2026-06-21
 */

#include "GridColoring/BoundCalculations.hpp"

int CalculateLowerBound(const std::vector<int>& colorCounts, int totalNodes, int uncoloredNodes) {
    // 1. Calculate floor and ceiling of the perfect average using pure integer math
    // This avoids expensive FPU (Floating Point Unit) context switches.
    int floorAverage = totalNodes / 4;
    int ceilAverage = (totalNodes + 3) / 4;

    // 2. Find the current min and max counts
    int minCount = GetMinCount(colorCounts);
    int maxCount = GetMaxCount(colorCounts);

    // 3. Calculate the two mathematical terms
    // Term 1: The rarest color needs to reach the average.
    int term1 = ceilAverage - (minCount + uncoloredNodes);
    
    // Term 2: The most common color already exceeds the average.
    int term2 = maxCount - floorAverage;

    // 4. Return the maximum of (0, term1, term2)
    return std::max({0, term1, term2});
}