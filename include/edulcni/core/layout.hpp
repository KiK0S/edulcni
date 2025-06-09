/**
 * Layout system for edulcni widgets
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace edulcni {
namespace internal {

class Widget;

enum class LayoutStrategy {
    VerticalStack,    // Stack widgets vertically (default)
    HorizontalStack,  // Stack widgets horizontally
    Grid,            // Arrange in a grid layout
    Free             // No automatic layout (manual positioning)
};

struct LayoutConfig {
    LayoutStrategy strategy = LayoutStrategy::VerticalStack;
    double canvas_width = 800.0;
    double canvas_height = 600.0;
    double margin = 20.0;       // Margin from canvas edges
    double padding = 20.0;      // Padding between widgets
    int grid_columns = 2;       // For grid layout
    bool center_horizontally = true;  // Center widgets horizontally when possible
    bool center_vertically = false;   // Center widgets vertically when possible
};

struct WidgetLayout {
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;
};

class LayoutManager {
private:
    LayoutConfig config_;
    
    // Calculate layout for vertical stack
    std::map<std::string, WidgetLayout> calculate_vertical_layout(
        const std::map<std::string, std::unique_ptr<Widget>>& widgets) const;
    
    // Calculate layout for horizontal stack  
    std::map<std::string, WidgetLayout> calculate_horizontal_layout(
        const std::map<std::string, std::unique_ptr<Widget>>& widgets) const;
    
    // Calculate layout for grid
    std::map<std::string, WidgetLayout> calculate_grid_layout(
        const std::map<std::string, std::unique_ptr<Widget>>& widgets) const;

public:
    LayoutManager() = default;
    explicit LayoutManager(const LayoutConfig& config) : config_(config) {}
    
    // Set layout configuration
    void set_config(const LayoutConfig& config) { config_ = config; }
    const LayoutConfig& config() const { return config_; }
    
    // Calculate positions for all non-contained widgets
    std::map<std::string, WidgetLayout> calculate_layout(
        const std::map<std::string, std::unique_ptr<Widget>>& widgets) const;
    
    // Apply calculated layout to widgets
    void apply_layout(
        const std::map<std::string, WidgetLayout>& layout,
        std::map<std::string, std::unique_ptr<Widget>>& widgets) const;
};

} // namespace internal

// Public API functions for layout control
void set_layout_strategy(internal::LayoutStrategy strategy);
void set_canvas_size(double width, double height);
void set_layout_margins(double margin, double padding);
void set_grid_columns(int columns);
void enable_auto_layout(bool enabled = true);
void disable_auto_layout();

// Manual positioning functions (automatically disable auto-layout)
void set_widget_position(const std::string& id, double x, double y);
void center_widget_horizontally(const std::string& id, double canvas_width = 800);
void center_widget_vertically(const std::string& id, double canvas_height = 600);
void center_widget(const std::string& id, double canvas_width = 800, double canvas_height = 600);
void position_widget_bottom_center(const std::string& id, double canvas_width = 800, double canvas_height = 600, double bottom_margin = 50);

} // namespace edulcni 