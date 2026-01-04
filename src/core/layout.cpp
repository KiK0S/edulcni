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
namespace {

double clamp_value(double value, double min_value, double max_value) {
    return std::max(min_value, std::min(max_value, value));
}

struct FlowItem {
    Widget* widget = nullptr;
    LayoutConstraints constraints;
    double measured_width = 0.0;
    double measured_height = 0.0;
    double resolved_width = 0.0;
    double resolved_height = 0.0;
};

} // namespace

LayoutConstraints LayoutManager::constraints_for(const std::string& id) const {
    auto it = constraints_.find(id);
    if (it != constraints_.end()) {
        return it->second;
    }
    return LayoutConstraints{};
}

std::map<std::string, WidgetLayout> LayoutManager::calculate_constraint_layout(
    const std::map<std::string, std::unique_ptr<Widget>>& widgets) const {

    std::map<std::string, WidgetLayout> layout;

    // Collect non-contained widgets
    std::vector<FlowItem> free_widgets;
    free_widgets.reserve(widgets.size());
    for (const auto& [id, widget] : widgets) {
        if (!widget->is_contained()) {
            free_widgets.push_back({widget.get(), constraints_for(id)});
        }
    }

    if (free_widgets.empty()) {
        return layout;
    }

    // Measure all widgets with intrinsic sizes
    for (auto& item : free_widgets) {
        item.widget->calculate_dimensions();
        item.measured_width = clamp_value(item.widget->width(), item.constraints.min_width, item.constraints.max_width);
        item.measured_height = clamp_value(item.widget->height(), item.constraints.min_height, item.constraints.max_height);

        if (item.constraints.aspect_ratio.has_value() && item.constraints.aspect_ratio.value() > 0.0) {
            // Maintain width/height ratio while respecting min/max bounds
            double ratio = item.constraints.aspect_ratio.value();
            item.measured_height = clamp_value(item.measured_width / ratio, item.constraints.min_height, item.constraints.max_height);
        }

        item.resolved_width = item.measured_width;
        item.resolved_height = item.measured_height;
    }

    const bool is_row_flow = config_.flow_direction == LayoutFlowDirection::Row;
    const double container_main = (is_row_flow ? config_.canvas_width : config_.canvas_height) - 2 * config_.margin;
    const double main_gap = is_row_flow ? config_.gap_x : config_.gap_y;
    const double cross_gap = is_row_flow ? config_.gap_y : config_.gap_x;

    auto exceeds_line_limit = [&](double projected_main, size_t items_in_line, int line_capacity) {
        if (!config_.wrap) return false;
        if (line_capacity > 0 && static_cast<int>(items_in_line) >= line_capacity) return true;
        return projected_main > container_main && !std::isinf(container_main);
    };

    double cursor_cross = config_.margin;
    std::vector<FlowItem> line_items;
    double line_main = 0.0;

    auto flush_line = [&]() {
        if (line_items.empty()) return;

        double total_main = 0.0;
        double total_flex = 0.0;
        double max_cross = 0.0;

        for (size_t idx = 0; idx < line_items.size(); ++idx) {
            auto& item = line_items[idx];
            double margin_main = is_row_flow ? item.constraints.margin_x * 2.0 : item.constraints.margin_y * 2.0;
            double margin_cross = is_row_flow ? item.constraints.margin_y * 2.0 : item.constraints.margin_x * 2.0;
            double main_size = (is_row_flow ? item.measured_width : item.measured_height) + margin_main;
            double cross_size = (is_row_flow ? item.measured_height : item.measured_width) + margin_cross;

            total_main += main_size;
            if (idx + 1 < line_items.size()) {
                total_main += main_gap;
            }
            max_cross = std::max(max_cross, cross_size);
            total_flex += item.constraints.flex_grow;
        }

        double available = container_main - total_main;
        if (available > 0 && total_flex > 0) {
            for (auto& item : line_items) {
                double main_size = is_row_flow ? item.measured_width : item.measured_height;
                double growth = available * (item.constraints.flex_grow / total_flex);
                main_size = clamp_value(main_size + growth,
                    is_row_flow ? item.constraints.min_width : item.constraints.min_height,
                    is_row_flow ? item.constraints.max_width : item.constraints.max_height);

                if (is_row_flow) {
                    item.resolved_width = main_size;
                    if (item.constraints.aspect_ratio.has_value() && item.constraints.aspect_ratio.value() > 0.0) {
                        item.resolved_height = clamp_value(main_size / item.constraints.aspect_ratio.value(),
                            item.constraints.min_height, item.constraints.max_height);
                    }
                } else {
                    item.resolved_height = main_size;
                    if (item.constraints.aspect_ratio.has_value() && item.constraints.aspect_ratio.value() > 0.0) {
                        item.resolved_width = clamp_value(main_size * item.constraints.aspect_ratio.value(),
                            item.constraints.min_width, item.constraints.max_width);
                    }
                }
            }
        }

        double offset_main = 0.0;
        if ((is_row_flow && config_.center_horizontally) || (!is_row_flow && config_.center_vertically)) {
            offset_main = std::max(0.0, available / 2.0);
        }

        double position_main = config_.margin + offset_main;
        for (auto& item : line_items) {
            double margin_main = is_row_flow ? item.constraints.margin_x : item.constraints.margin_y;
            double margin_cross = is_row_flow ? item.constraints.margin_y : item.constraints.margin_x;

            double main_size = is_row_flow ? item.resolved_width : item.resolved_height;
            double cross_size = is_row_flow ? item.resolved_height : item.resolved_width;

            double cross_offset = 0.0;
            if ((is_row_flow && config_.center_vertically) || (!is_row_flow && config_.center_horizontally)) {
                cross_offset = std::max(0.0, (max_cross - (cross_size + margin_cross * 2.0)) / 2.0);
            }

            WidgetLayout widget_layout;
            if (is_row_flow) {
                widget_layout.x = position_main + margin_main;
                widget_layout.y = cursor_cross + margin_cross + cross_offset;
                widget_layout.width = main_size;
                widget_layout.height = cross_size;
            } else {
                widget_layout.x = cursor_cross + margin_cross + cross_offset;
                widget_layout.y = position_main + margin_main;
                widget_layout.width = cross_size;
                widget_layout.height = main_size;
            }

            layout[item.widget->id()] = widget_layout;

            position_main += main_size + margin_main * 2.0 + main_gap;
        }

        cursor_cross += max_cross + cross_gap;
        line_items.clear();
        line_main = 0.0;
    };

    int line_capacity = (config_.strategy == LayoutStrategy::Grid && config_.grid_columns > 0)
        ? config_.grid_columns
        : -1;

    for (auto& item : free_widgets) {
        double margin_main = is_row_flow ? item.constraints.margin_x * 2.0 : item.constraints.margin_y * 2.0;
        double main_size = (is_row_flow ? item.measured_width : item.measured_height) + margin_main;
        size_t items_in_line = line_items.size();
        double projected_main = line_main + (items_in_line > 0 ? main_gap : 0.0) + main_size;

        if (exceeds_line_limit(projected_main, items_in_line, line_capacity)) {
            flush_line();
        }

        if (line_items.empty()) {
            line_main = main_size;
        } else {
            line_main += main_gap + main_size;
        }

        line_items.push_back(item);
    }

    flush_line();

    // Ensure widgets retain resolved sizes even when free positioning is used
    if (config_.strategy == LayoutStrategy::Free) {
        for (auto& [id, widget_layout] : layout) {
            if (auto it = widgets.find(id); it != widgets.end()) {
                widget_layout.x = it->second->x();
                widget_layout.y = it->second->y();
            }
        }
    }

    return layout;
}

std::map<std::string, WidgetLayout> LayoutManager::calculate_layout(
    const std::map<std::string, std::unique_ptr<Widget>>& widgets) const {

    // Derive flow direction from strategy for backward compatibility
    LayoutManager derived = *this;
    if (derived.config_.strategy == LayoutStrategy::VerticalStack) {
        derived.config_.flow_direction = LayoutFlowDirection::Column;
        derived.config_.wrap = false;
    } else if (derived.config_.strategy == LayoutStrategy::HorizontalStack) {
        derived.config_.flow_direction = LayoutFlowDirection::Row;
        derived.config_.wrap = false;
    } else if (derived.config_.strategy == LayoutStrategy::Grid) {
        derived.config_.flow_direction = LayoutFlowDirection::Row;
        derived.config_.wrap = true;
    }

    return derived.calculate_constraint_layout(widgets);
}

void LayoutManager::apply_layout(
    const std::map<std::string, WidgetLayout>& layout,
    std::map<std::string, std::unique_ptr<Widget>>& widgets) const {

    for (const auto& [widget_id, widget_layout] : layout) {
        auto it = widgets.find(widget_id);
        if (it != widgets.end()) {
            it->second->set_position(widget_layout.x, widget_layout.y);
            it->second->set_resolved_size(widget_layout.width, widget_layout.height);
        }
    }
}

void LayoutManager::set_constraints(const std::string& widget_id, const LayoutConstraints& constraints) {
    constraints_[widget_id] = constraints;
}

void LayoutManager::clear_constraints(const std::string& widget_id) {
    constraints_.erase(widget_id);
}

std::optional<LayoutConstraints> LayoutManager::get_constraints(const std::string& widget_id) const {
    auto it = constraints_.find(widget_id);
    if (it == constraints_.end()) return std::nullopt;
    return it->second;
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
    config.gap_x = padding;
    config.gap_y = padding;
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

// Constraint control helpers
void set_widget_min_size(const std::string& id, double min_width, double min_height) {
    auto& manager = internal::State::instance().layout_manager();
    auto constraints = manager.get_constraints(id).value_or(internal::LayoutConstraints{});
    constraints.min_width = min_width;
    constraints.min_height = min_height;
    manager.set_constraints(id, constraints);
}

void set_widget_max_size(const std::string& id, double max_width, double max_height) {
    auto& manager = internal::State::instance().layout_manager();
    auto constraints = manager.get_constraints(id).value_or(internal::LayoutConstraints{});
    constraints.max_width = max_width;
    constraints.max_height = max_height;
    manager.set_constraints(id, constraints);
}

void set_widget_flex(const std::string& id, double flex_grow, double flex_shrink) {
    auto& manager = internal::State::instance().layout_manager();
    auto constraints = manager.get_constraints(id).value_or(internal::LayoutConstraints{});
    constraints.flex_grow = flex_grow;
    constraints.flex_shrink = flex_shrink;
    manager.set_constraints(id, constraints);
}

void set_widget_aspect_ratio(const std::string& id, double aspect_ratio) {
    auto& manager = internal::State::instance().layout_manager();
    auto constraints = manager.get_constraints(id).value_or(internal::LayoutConstraints{});
    constraints.aspect_ratio = aspect_ratio;
    manager.set_constraints(id, constraints);
}

void set_widget_margins(const std::string& id, double margin_x, double margin_y) {
    auto& manager = internal::State::instance().layout_manager();
    auto constraints = manager.get_constraints(id).value_or(internal::LayoutConstraints{});
    constraints.margin_x = margin_x;
    constraints.margin_y = margin_y;
    manager.set_constraints(id, constraints);
}

void clear_widget_constraints(const std::string& id) {
    auto& manager = internal::State::instance().layout_manager();
    manager.clear_constraints(id);
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
