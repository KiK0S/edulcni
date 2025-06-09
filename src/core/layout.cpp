/**
 * Layout system implementation for edulcni widgets
 */

#include "edulcni/core/layout.hpp"
#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"
#include <algorithm>
#include <cmath>

namespace edulcni {
namespace internal {

std::map<std::string, WidgetLayout> LayoutManager::calculate_vertical_layout(
    const std::map<std::string, std::unique_ptr<Widget>>& widgets) const {
    
    std::map<std::string, WidgetLayout> layout;
    
    // Collect non-contained widgets
    std::vector<Widget*> free_widgets;
    for (const auto& [id, widget] : widgets) {
        if (!widget->is_contained()) {
            free_widgets.push_back(widget.get());
        }
    }
    
    if (free_widgets.empty()) {
        return layout;
    }
    
    // Calculate dimensions for all widgets
    for (auto* widget : free_widgets) {
        widget->calculate_dimensions();
    }
    
    // Calculate total height needed
    double total_height = 0;
    double max_width = 0;
    
    for (auto* widget : free_widgets) {
        total_height += widget->height();
        max_width = std::max(max_width, widget->width());
    }
    total_height += (free_widgets.size() - 1) * config_.padding;
    
    // Calculate starting Y position
    double start_y = config_.margin;
    if (config_.center_vertically) {
        start_y = (config_.canvas_height - total_height) / 2.0;
        start_y = std::max(start_y, config_.margin);
    }
    
    // Position widgets
    double current_y = start_y;
    for (auto* widget : free_widgets) {
        WidgetLayout widget_layout;
        widget_layout.width = widget->width();
        widget_layout.height = widget->height();
        widget_layout.y = current_y;
        
        // Calculate X position (centering or left-aligned)
        if (config_.center_horizontally) {
            widget_layout.x = (config_.canvas_width - widget->width()) / 2.0;
        } else {
            widget_layout.x = config_.margin;
        }
        
        layout[widget->id()] = widget_layout;
        current_y += widget->height() + config_.padding;
    }
    
    return layout;
}

std::map<std::string, WidgetLayout> LayoutManager::calculate_horizontal_layout(
    const std::map<std::string, std::unique_ptr<Widget>>& widgets) const {
    
    std::map<std::string, WidgetLayout> layout;
    
    // Collect non-contained widgets
    std::vector<Widget*> free_widgets;
    for (const auto& [id, widget] : widgets) {
        if (!widget->is_contained()) {
            free_widgets.push_back(widget.get());
        }
    }
    
    if (free_widgets.empty()) {
        return layout;
    }
    
    // Calculate dimensions for all widgets
    for (auto* widget : free_widgets) {
        widget->calculate_dimensions();
    }
    
    // Calculate total width needed
    double total_width = 0;
    double max_height = 0;
    
    for (auto* widget : free_widgets) {
        total_width += widget->width();
        max_height = std::max(max_height, widget->height());
    }
    total_width += (free_widgets.size() - 1) * config_.padding;
    
    // Calculate starting X position
    double start_x = config_.margin;
    if (config_.center_horizontally) {
        start_x = (config_.canvas_width - total_width) / 2.0;
        start_x = std::max(start_x, config_.margin);
    }
    
    // Position widgets
    double current_x = start_x;
    for (auto* widget : free_widgets) {
        WidgetLayout widget_layout;
        widget_layout.width = widget->width();
        widget_layout.height = widget->height();
        widget_layout.x = current_x;
        
        // Calculate Y position (centering or top-aligned)
        if (config_.center_vertically) {
            widget_layout.y = (config_.canvas_height - widget->height()) / 2.0;
        } else {
            widget_layout.y = config_.margin;
        }
        
        layout[widget->id()] = widget_layout;
        current_x += widget->width() + config_.padding;
    }
    
    return layout;
}

std::map<std::string, WidgetLayout> LayoutManager::calculate_grid_layout(
    const std::map<std::string, std::unique_ptr<Widget>>& widgets) const {
    
    std::map<std::string, WidgetLayout> layout;
    
    // Collect non-contained widgets
    std::vector<Widget*> free_widgets;
    for (const auto& [id, widget] : widgets) {
        if (!widget->is_contained()) {
            free_widgets.push_back(widget.get());
        }
    }
    
    if (free_widgets.empty()) {
        return layout;
    }
    
    // Calculate dimensions for all widgets
    for (auto* widget : free_widgets) {
        widget->calculate_dimensions();
    }
    
    int columns = std::max(1, config_.grid_columns);
    int rows = (free_widgets.size() + columns - 1) / columns;
    
    // Calculate cell dimensions
    double available_width = config_.canvas_width - 2 * config_.margin - (columns - 1) * config_.padding;
    double available_height = config_.canvas_height - 2 * config_.margin - (rows - 1) * config_.padding;
    
    double cell_width = available_width / columns;
    double cell_height = available_height / rows;
    
    // Position widgets in grid
    for (size_t i = 0; i < free_widgets.size(); ++i) {
        auto* widget = free_widgets[i];
        
        int row = i / columns;
        int col = i % columns;
        
        WidgetLayout widget_layout;
        widget_layout.width = widget->width();
        widget_layout.height = widget->height();
        
        // Calculate cell position
        double cell_x = config_.margin + col * (cell_width + config_.padding);
        double cell_y = config_.margin + row * (cell_height + config_.padding);
        
        // Center widget within cell
        widget_layout.x = cell_x + (cell_width - widget->width()) / 2.0;
        widget_layout.y = cell_y + (cell_height - widget->height()) / 2.0;
        
        layout[widget->id()] = widget_layout;
    }
    
    return layout;
}

std::map<std::string, WidgetLayout> LayoutManager::calculate_layout(
    const std::map<std::string, std::unique_ptr<Widget>>& widgets) const {
    
    switch (config_.strategy) {
        case LayoutStrategy::VerticalStack:
            return calculate_vertical_layout(widgets);
        case LayoutStrategy::HorizontalStack:
            return calculate_horizontal_layout(widgets);
        case LayoutStrategy::Grid:
            return calculate_grid_layout(widgets);
        case LayoutStrategy::Free:
            // Return empty layout - widgets keep their current positions
            return {};
        default:
            return calculate_vertical_layout(widgets);
    }
}

void LayoutManager::apply_layout(
    const std::map<std::string, WidgetLayout>& layout,
    std::map<std::string, std::unique_ptr<Widget>>& widgets) const {
    
    for (const auto& [widget_id, widget_layout] : layout) {
        auto it = widgets.find(widget_id);
        if (it != widgets.end()) {
            it->second->set_position(widget_layout.x, widget_layout.y);
        }
    }
}

} // namespace internal

// Public API implementations
void set_layout_strategy(internal::LayoutStrategy strategy) {
    auto& state = internal::State::instance();
    auto config = state.layout_manager().config();
    config.strategy = strategy;
    state.layout_manager().set_config(config);
}

void set_canvas_size(double width, double height) {
    auto& state = internal::State::instance();
    auto config = state.layout_manager().config();
    config.canvas_width = width;
    config.canvas_height = height;
    state.layout_manager().set_config(config);
}

void set_layout_margins(double margin, double padding) {
    auto& state = internal::State::instance();
    auto config = state.layout_manager().config();
    config.margin = margin;
    config.padding = padding;
    state.layout_manager().set_config(config);
}

void set_grid_columns(int columns) {
    auto& state = internal::State::instance();
    auto config = state.layout_manager().config();
    config.grid_columns = columns;
    state.layout_manager().set_config(config);
}

void enable_auto_layout(bool enabled) {
    auto& state = internal::State::instance();
    auto config = state.layout_manager().config();
    if (enabled && config.strategy == internal::LayoutStrategy::Free) {
        config.strategy = internal::LayoutStrategy::VerticalStack;
    } else if (!enabled) {
        config.strategy = internal::LayoutStrategy::Free;
    }
    state.layout_manager().set_config(config);
}

void disable_auto_layout() {
    enable_auto_layout(false);
}

// Manual positioning functions - these automatically disable auto-layout
void set_widget_position(const std::string& id, double x, double y) {
    // Switch to free layout to prevent auto-layout from overriding manual positioning
    auto& state = internal::State::instance();
    auto config = state.layout_manager().config();
    config.strategy = internal::LayoutStrategy::Free;
    state.layout_manager().set_config(config);
    
    // Set the widget position
    auto* widget = state.get_widget(id);
    if (widget) {
        widget->set_position(x, y);
    }
}

void center_widget_horizontally(const std::string& id, double canvas_width) {
    // Switch to free layout
    auto& state = internal::State::instance();
    auto config = state.layout_manager().config();
    config.strategy = internal::LayoutStrategy::Free;
    config.canvas_width = canvas_width; // Update canvas width for consistency
    state.layout_manager().set_config(config);
    
    auto* widget = state.get_widget(id);
    if (widget) {
        widget->calculate_dimensions();
        double x = (canvas_width - widget->width()) / 2;
        widget->set_position(x, widget->y());
    }
}

void center_widget_vertically(const std::string& id, double canvas_height) {
    // Switch to free layout
    auto& state = internal::State::instance();
    auto config = state.layout_manager().config();
    config.strategy = internal::LayoutStrategy::Free;
    config.canvas_height = canvas_height; // Update canvas height for consistency
    state.layout_manager().set_config(config);
    
    auto* widget = state.get_widget(id);
    if (widget) {
        widget->calculate_dimensions();
        double y = (canvas_height - widget->height()) / 2;
        widget->set_position(widget->x(), y);
    }
}

void center_widget(const std::string& id, double canvas_width, double canvas_height) {
    // Switch to free layout
    auto& state = internal::State::instance();
    auto config = state.layout_manager().config();
    config.strategy = internal::LayoutStrategy::Free;
    config.canvas_width = canvas_width;
    config.canvas_height = canvas_height;
    state.layout_manager().set_config(config);
    
    auto* widget = state.get_widget(id);
    if (widget) {
        widget->calculate_dimensions();
        double x = (canvas_width - widget->width()) / 2;
        double y = (canvas_height - widget->height()) / 2;
        widget->set_position(x, y);
    }
}

void position_widget_bottom_center(const std::string& id, double canvas_width, double canvas_height, double bottom_margin) {
    // Switch to free layout
    auto& state = internal::State::instance();
    auto config = state.layout_manager().config();
    config.strategy = internal::LayoutStrategy::Free;
    config.canvas_width = canvas_width;
    config.canvas_height = canvas_height;
    state.layout_manager().set_config(config);
    
    auto* widget = state.get_widget(id);
    if (widget) {
        widget->calculate_dimensions();
        double x = (canvas_width - widget->width()) / 2;
        double y = canvas_height - widget->height() - bottom_margin;
        widget->set_position(x, y);
    }
}

} // namespace edulcni 