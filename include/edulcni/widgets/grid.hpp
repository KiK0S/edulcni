/**
 * Generic grid widget for interactive grid-based visualizations
 */
#pragma once

#include <vector>
#include <functional>
#include <string>
#include <map>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"

namespace edulcni {

// Note: Grid functionality is now implemented using matrix widgets with custom click handling

// Legacy API for backward compatibility
class Grid {
public:
    // Create a grid with given dimensions
    static void create(const std::string& id, int rows, int cols);
    
    // Add a cell click handler for the given cell index
    static void register_cell_handler(const std::string& id, int cell_index, 
                                    std::function<void()> callback);
    // Clear all cells in the grid
    static void clear(const std::string& id);
};

} // namespace edulcni 