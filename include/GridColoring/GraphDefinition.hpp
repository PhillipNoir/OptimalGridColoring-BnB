#pragma once

/**
 * @file GraphDefinition.hpp
 * @brief Defines the graph structure for the grid coloring problem.
 * @author Sergio "Phillip Noir" Gonzalez
 * @date 2026-06-21
 * @version 1.0.0
 */

#include <vector>
#include <limits>
#include <utility>

/**
 * @class Grid1D
 * @brief Represents the grid as a 1D vector.
 * 
 * This class is a wrapper around a 1D vector that represents the grid.
 * It provides methods to get and set the color of a cell.
 */
class Grid1D{
    private:
        /**
         * @brief The width of the grid.
         */
        int width;

        /**
         * @brief The height of the grid.
         */
        int height;

        /**
         * @brief Vector with the cells of our grid using 1D format.
         */
        std::vector<int> data;

    public:
        /**
         * @brief Constructs a new Grid1D object.
         * 
         * @param w The width of the grid.
         * @param h The height of the grid.
         * @note We use an initialization list to initialize the members of the class, so we improve performance. Also, we initialize the data vector with -1, which represents an uncolored cell.
         * @note We don't use a class destructor because the vector will be destroyed automatically when the object is destroyed, following the Rule of Zero (Classes without manual resource management shouldn't have a user-defined destructor).
         */
        Grid1D(int w, int h) : width(w), height(h), data(w*h, -1){}

        /**
         * @brief Translates a 1D index back to 2D coordinates.
         * @param index The 1D index of the cell.
         * @return A std::pair where first is X (column) and second is Y (row).
         * @note The formula is x = index % width and y = index / width.
         */
        inline std::pair<int, int> getCoordinates(int index) const { return {index % width, index / width}; }

        /**
         * @brief Translates 2D coordinate to 1D vector index.
         * @param x The x-coordinate of the cell.
         * @param y The y-coordinate of the cell.
         * @return The index of the cell in the 1D vector.
         * @note We use inline to improve performance.
         * @note The formula is index = row * width + column.
         */
        inline int getIndex(int x, int y) const {return y*width + x;}

        /**
         * @brief Checks if a cell is valid.
         * @param x The x-coordinate of the cell.
         * @param y The y-coordinate of the cell.
         * @return True if the cell is valid, false otherwise.
         * @note We use inline to improve performance.
         */
        inline bool isValid(int x, int y) const {return x >= 0 && x < width && y >= 0 && y < height;}

        /**
         * @brief Sets the color of a cell.
         * @param x The x-coordinate of the cell.
         * @param y The y-coordinate of the cell.
         * @param color The color to set.
         */
        void setColor(int x, int y, int color) {data[getIndex(x, y)] = color;}

        /**
         * @brief Gets the color of a cell.
         * @param x The x-coordinate of the cell.
         * @param y The y-coordinate of the cell.
         * @return The color of the cell.
         */
        int getColor(int x, int y) const {return data[getIndex(x, y)];}

        /**
         * @brief Sets the color of a cell by index.
         * @param index The index of the cell.
         * @param color The color to set.
         */
        void setColorByIndex(int index, int color) { data[index] = color; }

        /**
         * @brief Gets the color of a cell by index.
         * @param index The index of the cell.
         * @return The color of the cell.
         */
        int getColorByIndex(int index) const { return data[index]; };

        /**
         * @brief Gets the width of the grid.
         * @return The width of the grid.
         */
        int getWidth() const { return width; }

        /**
         * @brief Gets the height of the grid.
         * @return The height of the grid.
         */
        int getHeight() const { return height; };
};

/**
 * @class SearchState
 * @brief Encapsulates the entire current context of the search, including the grid state, color tallies, and the best solution found.
 */
struct SearchState{
    /**
     * @brief The current grid configuration.
     */
    Grid1D grid;

    /**
     * @brief Counts of each color used so far.
     * @note The vector is of size 5 to allow 1-based indexing for colors (index 0 is ignored for convenience).
     */
    std::vector<int> colorCounts;

    /**
     * @brief The number of uncolored nodes. When this is 0, we have a complete coloring.
     */
    int uncoloredNodes;

    /**
     * @brief The current best difference. Our upper bound.
     * @note Initialize to int max so any valid diff will be smaller.
     */
    int currentBestDiff;

    /**
     * @brief The best grid. We need a "picture" of the best solution found so far to return at the end.
     */
    Grid1D bestGrid;

    /**
     * @brief Constructs a new SearchState object.
     * 
     * @param w The width of the grid.
     * @param h The height of the grid.
     * @note We use an initialization list to initialize the members of the class, so we improve performance.
     * @note We initialize the colorCounts vector with 5, we'll ignore the first index to avoid using substraction operations in every cycle.
     * @note We initialize the uncoloredNodes vector with w * h, which represents the total number of nodes.
     * @note We initialize the currentBestDiff with the maximum value of an int, so any valid diff will be smaller.
     */
    SearchState(int w, int h) : grid(w, h), bestGrid(w, h), colorCounts(5, 0), uncoloredNodes(w * h), currentBestDiff(std::numeric_limits<int>::max()) {}

};

/**
 * @brief Get the valid colors for a node.
 * 
 * @param nodeIndex The index of the node.
 * @param state The state of the search tree.
 * @return A vector of valid colors.
 */
std::vector<int> GetValidColors(int nodeIndex, const SearchState& state);