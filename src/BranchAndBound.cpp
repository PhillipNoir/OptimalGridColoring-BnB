/**
 * @file BranchAndBound.cpp
 * @brief Implementation of the Branch and Bound algorithm.
 * @author Sergio "Phillip Noir" Gonzalez
 * @date 2026-06-21
 */

#include "GridColoring/BranchAndBound.hpp"
#include "GridColoring/BoundCalculations.hpp"
#include <algorithm>
#include <numeric>

namespace {
    /**
     * @brief Helper function to calculate the Von Neumann degree of a node.
     * Corners = 2, Edges = 3, Internal = 4.
     */
    inline int GetNodeDegree(int index, const Grid1D& grid) {
        auto [x, y] = grid.getCoordinates(index);
        int degree = 4; // Start assuming it's an internal node
        
        // Subtract a degree for every wall it touches
        if (x == 0 || x == grid.getWidth() - 1) degree--;
        if (y == 0 || y == grid.getHeight() - 1) degree--;
        
        return degree;
    }
}

void RunGreedyInitialSolution(SearchState& state) {
    int width = state.grid.getWidth();
    int height = state.grid.getHeight();
    int totalNodes = width * height;

    // 1. Create a sequential list of all node indices (0 to N-1)
    std::vector<int> nodes(totalNodes);
    // Fill nodes with 0, 1, 2, ..., N-1
    std::iota(nodes.begin(), nodes.end(), 0); 

    // 2. Sort nodes by degree descending (Most restrictive nodes first)
    // We pass the grid from the main state by reference into the lambda
    std::sort(nodes.begin(), nodes.end(), [&state](int a, int b) {
        // Higher degree comes first (More constraints = Process first)
        return GetNodeDegree(a, state.grid) > GetNodeDegree(b, state.grid);
    });

    // 3. Create a parallel "Simulation State"
    // We run the greedy algorithm here to avoid corrupting the main search state
    SearchState greedyState(width, height);

    // 4. Color nodes sequentially based on our sorted priority
    // nodeIndex is the content of the element in the nodes vector, that the for loop uses.
    for (int nodeIndex : nodes) {
        
        // Find which colors don't conflict with neighbors in the simulation grid
        std::vector<int> validColors = GetValidColors(nodeIndex, greedyState);
        
        // Safety check: If a node has no valid colors left, the greedy fails.
        // This is extremely rare but theoretically possible on very tight grids.
        if (validColors.empty()) return; 

        // 5. Apply the "Greedy" Heuristic: Choose the color that minimizes immediate imbalance.
        // We pick the valid color that we have used the LEAST so far.
        int bestColor = validColors[0]; // Start with the first valid color, no matter if it is the best one.
        int minUsage = greedyState.colorCounts[bestColor]; // Start with the first valid color's usage count, no matter if it is the best one.
        
        // Check if the other valid colors are better than the one we started with, and if so, choose the best one. 
        for (size_t i = 1; i < validColors.size(); ++i) {
            int color = validColors[i];
            if (greedyState.colorCounts[color] < minUsage) {
                minUsage = greedyState.colorCounts[color];
                bestColor = color;
            }
        }
        
        // 6. Apply the decision to our simulation state
        greedyState.grid.setColorByIndex(nodeIndex, bestColor);
        greedyState.colorCounts[bestColor]++;
        greedyState.uncoloredNodes--;
    }

    // 7. Simulation complete. Calculate the achieved difference.
    int finalDiff = GetMaxCount(greedyState.colorCounts) - GetMinCount(greedyState.colorCounts);

    // 8. Commit the High Score (Upper Bound) back to the real state
    state.currentBestDiff = finalDiff;
    state.bestGrid = greedyState.grid; // Save the snapshot of this valid coloring
}

int SelectNextNodeMRV(const SearchState& state) {
    int bestNodeIndex = -1;
    
    // We start with 5 because the maximum possible remaining colors is 4.
    // This acts as our localized "infinity".
    int minRemainingValues = 5; 

    int totalNodes = state.grid.getWidth() * state.grid.getHeight();

    for (int i = 0; i < totalNodes; ++i) {
        // 1. Skip nodes that are already colored
        if (state.grid.getColorByIndex(i) != -1) {
            continue;
        }

        // 2. Query the Von Neumann neighborhood rules for this specific node
        int availableColors = GetValidColors(i, state).size();

        // 3. Evaluate if this node is more constrained than our previous worst
        if (availableColors < minRemainingValues) {
            minRemainingValues = availableColors;
            bestNodeIndex = i;

            // --- MICRO-OPTIMIZATION: THE ZERO-OPTION DEAD END ---
            // If a node has 0 available colors, it means this branch is already dead
            // (infeasibility). By breaking early and returning this node immediately,
            // we force the main Branch and Bound loop to evaluate it, realize it has
            // no valid iterations, and backtrack instantly without wasting CPU cycles
            // scanning the rest of the grid.
            if (minRemainingValues == 0) {
                break;
            }
        }
    }

    return bestNodeIndex;
}

void SortColorsByBalance(std::vector<int>& availableColors, const std::vector<int>& colorCounts) {
    std::sort(availableColors.begin(), availableColors.end(), [&colorCounts](int colorA, int colorB) {
        // We want ascending order: the color with the smaller count goes first
        return colorCounts[colorA] < colorCounts[colorB];
    });
}

// --- CORE RECURSIVE ALGORITHM ---

void ExecuteBranchAndBound(SearchState& state) {
    int totalNodes = state.grid.getWidth() * state.grid.getHeight();

    // 1. Lower Bound (LB)
    // O(1) mathematical prediction of the best possible outcome from here.
    int lb = CalculateLowerBound(state.colorCounts, totalNodes, state.uncoloredNodes);

    // 2. Pruning by LB (Pruning)
    // If our optimistic prediction is worse or equal to our current record, kill the branch.
    if (lb >= state.currentBestDiff) {
        return; 
    }

    // 3. Complete Solution Check
    // If there are no nodes left to color, we reached the bottom of a valid branch.
    if (state.uncoloredNodes == 0) {
        int diff = GetMaxCount(state.colorCounts) - GetMinCount(state.colorCounts);
        
        // Did we break the world record?
        if (diff < state.currentBestDiff) {
            state.currentBestDiff = diff;   // Update the Upper Bound
            state.bestGrid = state.grid;    // Take a snapshot of the grid
        }
        return;
    }

    // 4. Node Selection (MRV Heuristic)
    // Pick the most constrained node to fail fast.
    int nodeIndex = SelectNextNodeMRV(state);
    
    // Safety fallback (should conceptually never trigger if uncoloredNodes > 0)
    if (nodeIndex == -1) return; 

    // 5. Get Valid Colors (Feasibility)
    std::vector<int> availableColors = GetValidColors(nodeIndex, state);
    
    // If no colors are available, this branch is physically impossible. Prune it.
    if (availableColors.empty()) {
        return; 
    }

    // 6. Sort Colors by Balance (Balance Heuristic)
    // Prioritize colors we have used the least to find balanced solutions faster.
    SortColorsByBalance(availableColors, state.colorCounts);

    // 7. Branching and Backtracking (The DFS Loop)
    for (int color : availableColors) {
        
        // --- DESCEND (Apply decision) ---
        state.grid.setColorByIndex(nodeIndex, color);
        state.colorCounts[color]++;
        state.uncoloredNodes--;

        // Recursive call (Dive deeper into the tree)
        ExecuteBranchAndBound(state);

        // --- BACKTRACK (Undo decision) ---
        // Restore the exact state we had before this iteration to try the next color cleanly.
        state.grid.setColorByIndex(nodeIndex, -1);
        state.colorCounts[color]--;
        state.uncoloredNodes++;
    }
}


// --- PUBLIC API ---

SearchState SolveGrid(int width, int height) {
    // Initialize a fresh, clean state for the algorithm.
    // NOTE: Ensure your colorCounts is initialized to size 5 in the struct definition!
    SearchState state(width, height);

    // --- SYMMETRY REDUCTION (Reducción de Simetrías) ---
    // Since colors are interchangeable (1-2-3-4 is the same balance as 2-1-4-3),
    // we can forcefully pin the top-left corner to Color 1. 
    // This instantly divides the size of the search tree by 4.
    state.grid.setColorByIndex(0, 1);
    state.colorCounts[1]++;
    state.uncoloredNodes--;

    // --- INITIAL GREEDY SOLUTION ---
    // Get a fast valid solution to establish an initial Upper Bound (currentBestDiff).
    // Without this, the early tree would explore aimlessly.
    RunGreedyInitialSolution(state);

    // --- IGNITION ---
    // Start the deep search
    ExecuteBranchAndBound(state);

    // Return the state which now contains the 'bestGrid' and 'currentBestDiff'
    return state;
}