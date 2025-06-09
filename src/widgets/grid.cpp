#include "edulcni/widgets/grid.hpp"
#include "edulcni/widgets/matrix.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/events.hpp"
#include <map>
#include <functional>
#include <iostream>
namespace edulcni {

// Store grid metadata separately since we use matrix widgets for rendering
static std::map<std::string, std::pair<int, int>> grid_dimensions; // id -> (rows, cols)
static std::map<std::string, std::map<int, std::function<void()>>> grid_cell_handlers; // id -> (cell_index -> callback)

namespace internal {

// Constants for matrix cell dimensions (copied from MatrixWidgetBase)
static constexpr double MATRIX_CELL_WIDTH = 60;
static constexpr double MATRIX_CELL_HEIGHT = 40;
static constexpr double MATRIX_PADDING = 5;

// Function to handle grid clicks on matrix widgets
void handle_grid_matrix_click(const std::string& grid_id, double relative_x, double relative_y) {
    std::cout << "handle_grid_matrix_click called for " << grid_id 
              << " at relative (" << relative_x << ", " << relative_y << ")" << std::endl;
    
    auto dim_it = grid_dimensions.find(grid_id);
    if (dim_it == grid_dimensions.end()) {
        std::cout << "Grid dimensions not found for " << grid_id << std::endl;
        return;
    }
    
    auto [rows, cols] = dim_it->second;
    std::cout << "Grid dimensions: " << rows << "x" << cols << std::endl;
    
    // Calculate which cell was clicked using matrix cell dimensions
    double cell_width = MATRIX_CELL_WIDTH + MATRIX_PADDING;
    double cell_height = MATRIX_CELL_HEIGHT + MATRIX_PADDING;
    
    int col = (int)(relative_x / cell_width);
    int row = (int)(relative_y / cell_height);
    
    std::cout << "Calculated cell: row=" << row << ", col=" << col 
              << " (cell_size: " << cell_width << "x" << cell_height << ")" << std::endl;
    
    // Check bounds
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        int cell_index = row * cols + col;
        std::cout << "Valid cell index: " << cell_index << std::endl;
        
        // Look up and call the grid cell handler
        auto handlers_it = grid_cell_handlers.find(grid_id);
        if (handlers_it != grid_cell_handlers.end()) {
            auto& cell_handlers = handlers_it->second;
            auto handler_it = cell_handlers.find(cell_index);
            if (handler_it != cell_handlers.end()) {
                std::cout << "Calling cell handler for cell " << cell_index << std::endl;
                handler_it->second();
            } else {
                std::cout << "No handler registered for cell " << cell_index << std::endl;
            }
        } else {
            std::cout << "No cell handlers found for grid " << grid_id << std::endl;
        }
    } else {
        std::cout << "Cell coordinates out of bounds" << std::endl;
    }
}

} // namespace internal

void Grid::create(const std::string& id, int rows, int cols) {
    std::cout << "Creating grid: " << id << " (" << rows << "x" << cols << ")" << std::endl;
    
    // Store grid metadata
    grid_dimensions[id] = {rows, cols};
    
    // Create matrix representation (this creates the actual widget)
    std::vector<std::vector<char>> emptyGrid(rows, std::vector<char>(cols, '.'));
    matrix_widget(id, emptyGrid);
    
    // Verify widget was created
    auto* widget = internal::State::instance().get_widget(id);
    if (widget) {
        std::cout << "Widget created successfully for " << id << std::endl;
        std::cout << "Initial widget bounds: x=" << widget->x() << ", y=" << widget->y() 
                  << ", w=" << widget->width() << ", h=" << widget->height() << std::endl;
    } else {
        std::cout << "ERROR: Widget creation failed for " << id << std::endl;
    }
    
    // Register grid-specific click handling  
    std::cout << "Registering canvas_click handler for grid " << id << std::endl;
    register_event_callback("canvas_click", [id](const EventData& event) {
        std::cout << "CANVAS CLICK HANDLER CALLED for grid " << id << std::endl;
        auto* widget = internal::State::instance().get_widget(id);
        if (widget) {
            // Debug logging
            std::cout << "Grid " << id << " canvas click at (" << event.x << ", " << event.y << ")" << std::endl;
            std::cout << "Widget bounds: x=" << widget->x() << ", y=" << widget->y() 
                      << ", w=" << widget->width() << ", h=" << widget->height() << std::endl;
            
            if (widget->contains_point(event.x, event.y)) {
                std::cout << "Click is within grid bounds, handling..." << std::endl;
                double relative_x = event.x - widget->x();
                double relative_y = event.y - widget->y();
                internal::handle_grid_matrix_click(id, relative_x, relative_y);
            } else {
                std::cout << "Click is outside grid bounds" << std::endl;
            }
        } else {
            std::cout << "No widget found for grid " << id << std::endl;
        }
    });
    
    // Register the unified widget click handler for backwards compatibility
    register_widget_click_handler(id);
}

void Grid::register_cell_handler(const std::string& id, int cell_index, 
                               std::function<void()> callback) {
    // Store the cell handler in our metadata
    grid_cell_handlers[id][cell_index] = callback;
}

void Grid::clear(const std::string& id) {
    auto dim_it = grid_dimensions.find(id);
    if (dim_it != grid_dimensions.end()) {
        auto [rows, cols] = dim_it->second;
        
        // Clear cell handlers
        grid_cell_handlers[id].clear();
        
        // Clear matrix representation
        std::vector<std::vector<char>> emptyGrid(rows, std::vector<char>(cols, '.'));
        matrix_widget(id, emptyGrid);
        matrix_clear_highlights(id);
    }
}

} // namespace edulcni 