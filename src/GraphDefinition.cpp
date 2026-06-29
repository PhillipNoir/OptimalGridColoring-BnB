/**
 * @file GraphDefinition.cpp
 * @brief Implementation of the graph structure for the grid coloring problem.
 * @author Sergio "Phillip Noir" Gonzalez
 * @date 2026-06-21
 */

#include "GridColoring/GraphDefinition.hpp"

std::vector<int> GetValidColors(int nodeIndex, const SearchState& state){
    // We use a boolean array for O(1) lookups. 
    // Index 0 is ignored. Indices 1-4 represent our valid colors.
    bool colorIsFree[5] = {false, true, true, true, true}; 

    int width = state.grid.getWidth();
    int height = state.grid.getHeight();

    // Translate 1D index to 2D coordinates to check grid boundaries
    auto [x, y] = state.grid.getCoordinates(nodeIndex);

    // 1. Check UP neighbor
    if (y > 0) { 
        // We subtract the width to get the index of the cell above.
        int upIndex = nodeIndex - width;
        int color = state.grid.getColorByIndex(upIndex);
        if (color != -1) colorIsFree[color] = false;
    }

    // 2. Check DOWN neighbor
    if (y < height - 1) { 
        // We add the width to get the index of the cell below.
        int downIndex = nodeIndex + width;
        int color = state.grid.getColorByIndex(downIndex);
        if (color != -1) colorIsFree[color] = false;
    }

    // 3. Check LEFT neighbor
    if (x > 0) { 
        // We subtract 1 to get the index of the cell to the left.
        int leftIndex = nodeIndex - 1;
        int color = state.grid.getColorByIndex(leftIndex);
        if (color != -1) colorIsFree[color] = false;
    }

    // 4. Check RIGHT neighbor
    if (x < width - 1) { 
        // We add 1 to get the index of the cell to the right.
        int rightIndex = nodeIndex + 1;
        int color = state.grid.getColorByIndex(rightIndex);
        if (color != -1) colorIsFree[color] = false;
    }

    // Pre-allocate memory to prevent dynamic reallocations during the search
    std::vector<int> validColors;
    validColors.reserve(4); 
    
    for (int i = 1; i <= 4; ++i) {
        if (colorIsFree[i]) {
            validColors.push_back(i);
        }
    }

    return validColors;
}