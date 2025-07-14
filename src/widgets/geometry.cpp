/**
 * Geometry widget implementation for edulcni
 */
#include "edulcni/widgets/geometry.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

namespace edulcni {

// ============================================================================
// NON-TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

void create_geometry_widget(const std::string& id, double width, double height, double grid_spacing) {
    auto& state = internal::State::instance();
    auto widget = state.get_or_create_widget<internal::GeometryWidgetBase>(id, width, height);
    widget->set_grid_spacing(grid_spacing);
}

void clear_geometry(const std::string& widget_id) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->clear_all_elements();
    }
}

void clear_highlights(const std::string& widget_id) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->clear_highlights();
    }
}

void set_grid(const std::string& widget_id, bool enabled, double spacing, const std::string& color) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->set_grid_enabled(enabled);
        widget->set_grid_spacing(spacing);
        widget->set_grid_color(color);
    }
}

void set_coordinate_system(const std::string& widget_id, bool show_axes,
                          double origin_x, double origin_y, double scale_x, double scale_y) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->set_axes_enabled(show_axes);
        if (origin_x >= 0 && origin_y >= 0) {
            widget->set_origin(origin_x, origin_y);
        }
        widget->set_scale(scale_x, scale_y);
    }
}

void enable_zoom_pan(const std::string& widget_id, bool enabled, double min_zoom, double max_zoom) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->set_zoom_pan_enabled(enabled);
        widget->set_zoom_limits(min_zoom, max_zoom);
    }
}

void auto_scale_view(const std::string& widget_id, double padding_factor) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->auto_scale(padding_factor);
    }
}

void register_geometry_click_handler(const std::string& widget_id, 
                                    std::function<void(double x, double y)> callback) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->set_click_handler(std::move(callback));
    }
}

// ============================================================================
// WIDGET METHOD IMPLEMENTATIONS
// ============================================================================

namespace internal {

std::unique_ptr<render::Element> GeometryWidgetBase::render() const {
    auto group = std::make_unique<render::Group>();
    
    // Render grid and axes in screen coordinates (not affected by world transform)
    if (grid_enabled_) {
        render_grid(group.get());
    }
    if (axes_enabled_) {
        render_axes(group.get());
    }
    
    // Create a separate group for geometric elements
    auto elements_group = std::make_unique<render::Group>();
    
    // Apply combined transformation: zoom-pan + coordinate system
    double final_scale_x = scale_x_;
    double final_scale_y = scale_y_;
    double final_origin_x = origin_x_;
    double final_origin_y = origin_y_;
    
    if (zoom_pan_.enabled) {
        final_scale_x *= zoom_pan_.scale_x;
        final_scale_y *= zoom_pan_.scale_y;
        final_origin_x += zoom_pan_.offset_x;
        final_origin_y += zoom_pan_.offset_y;
    }
    
    elements_group->add(std::make_unique<render::Transform>(
        final_scale_x, 0, 0, final_scale_y, final_origin_x, final_origin_y
    ));
    
    // Render all geometric elements within this transformed group
    for (const auto& element : elements_) {
        render_element(elements_group.get(), element);
    }
    
    group->add(std::move(elements_group));
    
    return group;
}

void GeometryWidgetBase::render_grid(render::Group* group) const {
    if (!grid_enabled_ || grid_spacing_ <= 0) return;
    
    render::Color grid_color(224, 224, 224); // Default light gray
    if (grid_color_ == "#d0d0d0") {
        grid_color = render::Color(208, 208, 208);
    }
    
    for (double x = 0; x <= width_; x += grid_spacing_) {
        group->add(std::make_unique<render::Line>(x, 0, x, height_, grid_color, 0.5));
    }
    
    for (double y = 0; y <= height_; y += grid_spacing_) {
        group->add(std::make_unique<render::Line>(0, y, width_, y, grid_color, 0.5));
    }
}

void GeometryWidgetBase::render_axes(render::Group* group) const {
    if (!axes_enabled_) return;
    
    render::Color axis_color = render::Color::Black();
    
    group->add(std::make_unique<render::Line>(0, origin_y_, width_, origin_y_, axis_color, 1.0));
    group->add(std::make_unique<render::Line>(origin_x_, 0, origin_x_, height_, axis_color, 1.0));
    
    double tick_length = 8.0;
    int num_ticks = 5;

    for (int i = 0; i <= num_ticks; ++i) {
        double x = i * (width_ / num_ticks);
        group->add(std::make_unique<render::Line>(x, origin_y_ - tick_length/2, x, origin_y_ + tick_length/2, axis_color, 1.0));
        double world_x = (x - origin_x_) / scale_x_;
        group->add(std::make_unique<render::Text>(x, origin_y_ + 20, std::to_string(static_cast<int>(world_x)), "10px Arial", axis_color, "center"));
    }
    
    for (int i = 0; i <= num_ticks; ++i) {
        double y = i * (height_ / num_ticks);
        group->add(std::make_unique<render::Line>(origin_x_ - tick_length/2, y, origin_x_ + tick_length/2, y, axis_color, 1.0));
        double world_y = (y - origin_y_) / scale_y_;
        group->add(std::make_unique<render::Text>(origin_x_ - 10, y, std::to_string(static_cast<int>(world_y)), "10px Arial", axis_color, "right"));
    }
    
    group->add(std::make_unique<render::Text>(width_ - 20, origin_y_ - 10, "x", "12px Arial", axis_color, "center"));
    group->add(std::make_unique<render::Text>(origin_x_ + 10, 20, "y", "12px Arial", axis_color, "center"));
}

void GeometryWidgetBase::render_element(render::Group* group, const GeometryElement& element) const {
     switch (element.type) {
        case GeometryElement::Type::POINT: {
            if (element.data.size() >= 3) {
                double world_x = element.data[0];
                double world_y = element.data[1];
                double radius = element.data[2];
                
                render::Color point_color = render::Color::Black();
                if (element.color == "red") point_color = render::Color::Red();
                else if (element.color == "blue") point_color = render::Color::Blue();
                else if (element.color == "green") point_color = render::Color::Green();
                else if (element.color == "yellow") point_color = render::Color::Yellow();
                else if (element.color == "purple") point_color = render::Color(128, 0, 128);
                else if (element.color == "cyan") point_color = render::Color(0, 255, 255);
                else if (element.color == "gray") point_color = render::Color(128, 128, 128);
                
                group->add(std::make_unique<render::Circle>(
                    world_x, world_y, radius, point_color, render::Color::Black(), 1.0
                ));
                
                if (!element.label.empty()) {
                    group->add(std::make_unique<render::Text>(
                        world_x + radius + 5, world_y, element.label, "12px Arial", render::Color::Black(), "left"
                    ));
                }
            }
            break;
        }
        
        case GeometryElement::Type::SEGMENT: {
            if (element.data.size() >= 4) {
                double world_x1 = element.data[0];
                double world_y1 = element.data[1];
                double world_x2 = element.data[2];
                double world_y2 = element.data[3];
                double width = element.data.size() > 4 ? element.data[4] : 1.0;
                
                render::Color line_color = render::Color::Black();
                if (element.color == "red") line_color = render::Color::Red();
                else if (element.color == "blue") line_color = render::Color::Blue();
                else if (element.color == "green") line_color = render::Color::Green();
                else if (element.color == "purple") line_color = render::Color(128, 0, 128);
                else if (element.color == "orange") line_color = render::Color(255, 165, 0);
                
                group->add(std::make_unique<render::Line>(
                    world_x1, world_y1, world_x2, world_y2, line_color, width
                ));
            }
            break;
        }
        
        case GeometryElement::Type::LINE: {
            if (element.data.size() >= 4) {
                double world_x = element.data[0];
                double world_y = element.data[1];
                double dx = element.data[2];
                double dy = element.data[3];
                double width = element.data.size() > 4 ? element.data[4] : 1.0;
                
                double length = 1000.0;
                double world_x1 = world_x - length * dx;
                double world_y1 = world_y - length * dy;
                double world_x2 = world_x + length * dx;
                double world_y2 = world_y + length * dy;
                
                render::Color line_color = render::Color::Black();
                if (element.color == "red") line_color = render::Color::Red();
                else if (element.color == "blue") line_color = render::Color::Blue();
                else if (element.color == "green") line_color = render::Color::Green();
                
                group->add(std::make_unique<render::Line>(
                    world_x1, world_y1, world_x2, world_y2, line_color, width
                ));
            }
            break;
        }
        
        case GeometryElement::Type::CIRCLE: {
            if (element.data.size() >= 3) {
                double world_x = element.data[0];
                double world_y = element.data[1];
                double radius = element.data[2];
                
                render::Color fill_color = render::Color(255, 255, 255, 0); 
                render::Color stroke_color = render::Color::Black();
                
                if (element.stroke_color == "red") stroke_color = render::Color::Red();
                else if (element.stroke_color == "blue") stroke_color = render::Color::Blue();
                else if (element.stroke_color == "green") stroke_color = render::Color::Green();
                else if (element.stroke_color == "brown") stroke_color = render::Color(139, 69, 19);
                else if (element.stroke_color == "gold") stroke_color = render::Color(255, 215, 0);
                
                if (element.fill) {
                    if (element.color.rfind("rgba", 0) == 0) {
                        // basic parsing for example colors
                        if (element.color.find("255, 0, 0") != std::string::npos) fill_color = render::Color(255, 0, 0, 76); // 0.3 alpha
                        else if (element.color.find("0, 255, 0") != std::string::npos) fill_color = render::Color(0, 255, 0, 76);
                        else if (element.color.find("0, 0, 255") != std::string::npos) fill_color = render::Color(0, 0, 255, 51); // 0.2 alpha
                        else if (element.color.find("255, 165, 0") != std::string::npos) fill_color = render::Color(255, 165, 0, 51);
                        else if (element.color.find("255, 255, 0") != std::string::npos) fill_color = render::Color(255, 255, 0, 102); // 0.4 alpha
                    }
                }
                
                group->add(std::make_unique<render::Circle>(
                    world_x, world_y, radius, fill_color, stroke_color, element.stroke_width
                ));
            }
            break;
        }
        
        case GeometryElement::Type::POLYGON: {
            if (element.data.size() >= 6) { 
                render::Color stroke_color = render::Color::Black();
                if (element.stroke_color == "red") stroke_color = render::Color::Red();
                else if (element.stroke_color == "blue") stroke_color = render::Color::Blue();
                else if (element.stroke_color == "green") stroke_color = render::Color::Green();
                else if (element.stroke_color == "gold") stroke_color = render::Color(255, 215, 0);
                
                for (size_t i = 0; i < element.data.size() - 2; i += 2) {
                    group->add(std::make_unique<render::Line>(
                        element.data[i], element.data[i+1], element.data[i+2], element.data[i+3], 
                        stroke_color, element.stroke_width
                    ));
                }
                
                if (element.data.size() >= 4) {
                    group->add(std::make_unique<render::Line>(
                        element.data.back(), element.data[element.data.size()-2], element.data[0], element.data[1],
                        stroke_color, element.stroke_width
                    ));
                }
            }
            break;
        }
        
        case GeometryElement::Type::HALFPLANE: {
             if (element.data.size() >= 4) {
                double world_x = element.data[0];
                double world_y = element.data[1];
                double dx = element.data[2];
                double dy = element.data[3];
                
                render::Color boundary_color = render::Color::Black();
                if (element.stroke_color == "blue") boundary_color = render::Color::Blue();
                else if (element.stroke_color == "orange") boundary_color = render::Color(255, 165, 0);
                
                double length = 2000.0;
                double world_x1 = world_x - length * dy;
                double world_y1 = world_y + length * dx;
                double world_x2 = world_x + length * dy;
                double world_y2 = world_y - length * dx;
                
                group->add(std::make_unique<render::Line>(
                    world_x1, world_y1, world_x2, world_y2, boundary_color, 1.0
                ));
            }
            break;
        }
        
        case GeometryElement::Type::VECTOR: {
            if (element.data.size() >= 6) {
                double world_x = element.data[0];
                double world_y = element.data[1];
                double dx = element.data[2];
                double dy = element.data[3];
                double width = element.data[4];
                double head_size = element.data[5];
                
                render::Color vector_color = render::Color::Red();
                if (element.color == "purple") vector_color = render::Color(128, 0, 128);
                else if (element.color == "orange") vector_color = render::Color(255, 165, 0);
                
                double world_x2 = world_x + dx;
                double world_y2 = world_y + dy;
                
                group->add(std::make_unique<render::Line>(
                    world_x, world_y, world_x2, world_y2, vector_color, width
                ));
                
                double angle = atan2(dy, dx);
                double arrow_angle1 = angle + M_PI * 5.0 / 6.0;
                double arrow_angle2 = angle - M_PI * 5.0 / 6.0;
                
                double world_x3 = world_x2 + head_size * cos(arrow_angle1);
                double world_y3 = world_y2 + head_size * sin(arrow_angle1);
                double world_x4 = world_x2 + head_size * cos(arrow_angle2);
                double world_y4 = world_y2 + head_size * sin(arrow_angle2);
                
                group->add(std::make_unique<render::Line>(
                    world_x2, world_y2, world_x3, world_y3, vector_color, width
                ));
                
                group->add(std::make_unique<render::Line>(
                    world_x2, world_y2, world_x4, world_y4, vector_color, width
                ));
                
                if (!element.label.empty()) {
                    group->add(std::make_unique<render::Text>(
                        world_x2 + 5, world_y2, element.label, "12px Arial", render::Color::Black(), "left"
                    ));
                }
            }
            break;
        }
        
        case GeometryElement::Type::LABEL:
            break;
    }
}

} // namespace internal

} // namespace edulcni 