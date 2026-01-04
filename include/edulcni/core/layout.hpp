/**
 * Layout system for edulcni widgets
 */
#pragma once

#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace edulcni {
namespace internal {

class Widget;

enum class LayoutStrategy {
    VerticalStack,    // Stack widgets vertically (default)
    HorizontalStack,  // Stack widgets horizontally
    Grid,            // Arrange in a grid layout
    Free             // No automatic layout (manual positioning)
};

enum class LayoutFlowDirection {
    Row,
    Column
};

struct LayoutConfig {
    LayoutStrategy strategy = LayoutStrategy::VerticalStack;
    double canvas_width = 800.0;
    double canvas_height = 600.0;
    double margin = 20.0;       // Margin from canvas edges
    double padding = 20.0;      // Legacy field kept for compatibility
    double gap_x = 20.0;        // Horizontal gap between widgets
    double gap_y = 20.0;        // Vertical gap between widgets
    int grid_columns = 2;       // For grid layout
    bool center_horizontally = true;  // Center widgets horizontally when possible
    bool center_vertically = false;   // Center widgets vertically when possible
    LayoutFlowDirection flow_direction = LayoutFlowDirection::Row;
    bool wrap = true;
};

struct LayoutConstraints {
    double min_width = 0.0;
    double min_height = 0.0;
    double max_width = std::numeric_limits<double>::infinity();
    double max_height = std::numeric_limits<double>::infinity();
    double flex_grow = 0.0;
    double flex_shrink = 0.0;
    std::optional<double> aspect_ratio; // width / height
    double margin_x = 0.0;
    double margin_y = 0.0;
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
    std::map<std::string, LayoutConstraints> constraints_;

    LayoutConstraints constraints_for(const std::string& id) const;
    std::map<std::string, WidgetLayout> calculate_constraint_layout(
        const std::map<std::string, std::unique_ptr<Widget>>& widgets) const;

public:
    LayoutManager() = default;
    explicit LayoutManager(const LayoutConfig& config) : config_(config) {}
    
    // Set layout configuration
    void set_config(const LayoutConfig& config) { config_ = config; }
    const LayoutConfig& config() const { return config_; }

    // Widget constraints
    void set_constraints(const std::string& widget_id, const LayoutConstraints& constraints);
    void clear_constraints(const std::string& widget_id);
    std::optional<LayoutConstraints> get_constraints(const std::string& widget_id) const;
    
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

// Constraint controls
void set_widget_min_size(const std::string& id, double min_width, double min_height);
void set_widget_max_size(const std::string& id, double max_width, double max_height);
void set_widget_flex(const std::string& id, double flex_grow, double flex_shrink = 0.0);
void set_widget_aspect_ratio(const std::string& id, double aspect_ratio);
void set_widget_margins(const std::string& id, double margin_x, double margin_y);
void clear_widget_constraints(const std::string& id);

// Manual positioning functions (automatically disable auto-layout)
void set_widget_position(const std::string& id, double x, double y);
void center_widget_horizontally(const std::string& id, double canvas_width = 800);
void center_widget_vertically(const std::string& id, double canvas_height = 600);
void center_widget(const std::string& id, double canvas_width = 800, double canvas_height = 600);
void position_widget_bottom_center(const std::string& id, double canvas_width = 800, double canvas_height = 600, double bottom_margin = 50);

} // namespace edulcni 