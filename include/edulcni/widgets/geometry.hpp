/**
 * Geometry widget for 2D geometric visualizations
 * Supports points, lines, segments, circles, polygons, halfplanes, vectors, and labels
 * Includes zoom-pan functionality and auto-scaling
 */
#ifndef EDULCNI_WIDGETS_GEOMETRY_HPP
#define EDULCNI_WIDGETS_GEOMETRY_HPP

#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"
#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <limits>
#include <algorithm>

namespace edulcni {

// Forward declarations
namespace internal {
    class GeometryWidgetBase;
}

DEFINE_WIDGET_TYPE(Geometry)

// ============================================================================
// PUBLIC API
// ============================================================================

GeometryIDType create_geometry_widget(const std::string& id, double width, double height, double grid_spacing = 20.0);

template<typename PointType>
void draw_point(const GeometryIDType& widget_id, const PointType& point, 
                const std::string& color = "black", double radius = 3.0,
                const std::string& label = "");

template<typename PointType>
void draw_points(const GeometryIDType& widget_id, const std::vector<PointType>& points,
                 const std::string& color = "black", double radius = 3.0,
                 const std::vector<std::string>& labels = {});

template<typename PointType>
void draw_segment(const GeometryIDType& widget_id, const PointType& start, const PointType& end,
                  const std::string& color = "black", double width = 1.0,
                  const std::string& style = "solid");

template<typename PointType, typename VectorType>
void draw_line(const GeometryIDType& widget_id, const PointType& point, const VectorType& direction,
               const std::string& color = "black", double width = 1.0,
               const std::string& style = "solid");

template<typename PointType>
void draw_circle(const GeometryIDType& widget_id, const PointType& center, double radius,
                 const std::string& color = "transparent", const std::string& stroke_color = "black",
                 double stroke_width = 1.0, bool fill = false);

template<typename PointType>
void draw_polygon(const GeometryIDType& widget_id, const std::vector<PointType>& vertices,
                  const std::string& color = "transparent", const std::string& stroke_color = "black",
                  double stroke_width = 1.0, bool fill = false);

template<typename PointType, typename VectorType>
void draw_halfplane(const GeometryIDType& widget_id, const PointType& boundary_point, 
                    const VectorType& direction, const std::string& color = "rgba(173, 216, 230, 0.3)",
                    const std::string& boundary_color = "black");

template<typename PointType, typename VectorType>
void draw_vector(const GeometryIDType& widget_id, const PointType& origin, const VectorType& direction,
                 const std::string& color = "red", double width = 2.0, double head_size = 8.0,
                 const std::string& label = "");

template<typename PointType>
void highlight_point(const GeometryIDType& widget_id, const PointType& point, 
                     bool highlight = true, const std::string& color = "yellow");

void clear_geometry(const GeometryIDType& widget_id);
void clear_highlights(const GeometryIDType& widget_id);

void set_grid(const GeometryIDType& widget_id, bool enabled = true, double spacing = 20.0,
              const std::string& color = "#e0e0e0");

void set_coordinate_system(const GeometryIDType& widget_id, bool show_axes = true,
                          double origin_x = -1, double origin_y = -1,
                          double scale_x = 1.0, double scale_y = 1.0);

void enable_zoom_pan(const GeometryIDType& widget_id, bool enabled = true, 
                     double min_zoom = 0.1, double max_zoom = 10.0);

void auto_scale_view(const GeometryIDType& widget_id, double padding_factor = 0.1);

void register_geometry_click_handler(const GeometryIDType& widget_id, 
                                    std::function<void(double x, double y)> callback);


namespace internal {

// ============================================================================
// DATA STRUCTURES
// ============================================================================

struct GeometryElement {
    enum class Type {
        POINT, SEGMENT, LINE, CIRCLE, POLYGON, HALFPLANE, VECTOR, LABEL
    };
    
    Type type;
    std::vector<double> data;
    std::string color;
    std::string stroke_color;
    double stroke_width;
    bool highlighted;
    std::string label;
    std::string style;
    bool fill;
    
    GeometryElement(Type t) : type(t), stroke_width(1.0), highlighted(false), fill(false) {}
};

struct ZoomPanState {
    double scale_x = 1.0;
    double scale_y = 1.0;
    double offset_x = 0.0;
    double offset_y = 0.0;
    double min_zoom = 0.1;
    double max_zoom = 10.0;
    bool enabled = false;
    bool is_dragging = false;
    double last_mouse_x = 0.0;
    double last_mouse_y = 0.0;
    
    std::pair<double, double> screen_to_world(double screen_x, double screen_y) const {
        return {
            (screen_x - offset_x) / scale_x,
            (screen_y - offset_y) / scale_y
        };
    }
    
    void zoom_at(double center_x, double center_y, double zoom_factor) {
        if (!enabled) return;
        double new_scale_x = scale_x * zoom_factor;
        double new_scale_y = scale_y * zoom_factor;
        if (new_scale_x >= min_zoom && new_scale_x <= max_zoom &&
            new_scale_y >= min_zoom && new_scale_y <= max_zoom) {
            offset_x = center_x - (center_x - offset_x) * zoom_factor;
            offset_y = center_y - (center_y - offset_y) * zoom_factor;
            scale_x = new_scale_x;
            scale_y = new_scale_y;
        }
    }
    
    void pan(double delta_x, double delta_y) {
        if (!enabled) return;
        offset_x += delta_x;
        offset_y += delta_y;
    }
};

// ============================================================================
// WIDGET IMPLEMENTATION
// ============================================================================

class GeometryWidgetBase : public Widget {
protected:
    std::vector<GeometryElement> elements_;
    bool grid_enabled_ = true;
    double grid_spacing_ = 20.0;
    std::string grid_color_ = "#e0e0e0";
    bool axes_enabled_ = true;
    double origin_x_ = -1, origin_y_ = -1;  // -1 means center
    double scale_x_ = 1.0, scale_y_ = 1.0;
    ZoomPanState zoom_pan_;
    std::function<void(double, double)> click_handler_;
    double min_x_ = std::numeric_limits<double>::max();
    double min_y_ = std::numeric_limits<double>::max();
    double max_x_ = std::numeric_limits<double>::lowest();
    double max_y_ = std::numeric_limits<double>::lowest();
    bool has_data_ = false;

public:
    GeometryWidgetBase(std::string id, double width, double height) 
        : Widget(std::move(id)) {
        width_ = width;
        height_ = height;
        origin_x_ = width_ / 2;
        origin_y_ = height_ / 2;
    }
    
    void update_bounds(double x, double y) {
        if (x < min_x_) min_x_ = x;
        if (y < min_y_) min_y_ = y;
        if (x > max_x_) max_x_ = x;
        if (y > max_y_) max_y_ = y;
        has_data_ = true;
    }

    void auto_scale(double padding_factor = 0.1) {
        if (!has_data_) return;
        
        double data_width = max_x_ - min_x_;
        double data_height = max_y_ - min_y_;
        
        if (data_width <= 0 || data_height <= 0) return;
        
        double padding_x = data_width * padding_factor;
        double padding_y = data_height * padding_factor;
        
        double view_width = width_ - 2 * padding_x;
        double view_height = height_ - 2 * padding_y;

        if (view_width <= 0 || view_height <= 0) return;
        
        double scale_val = std::min(view_width / data_width, view_height / data_height);
        
        scale_x_ = scale_val;
        scale_y_ = -scale_val; // Y-axis flipped for mathematical coordinates
        
        double center_x = (min_x_ + max_x_) / 2.0;
        double center_y = (min_y_ + max_y_) / 2.0;
        
        origin_x_ = width_ / 2.0 - center_x * scale_x_;
        origin_y_ = height_ / 2.0 - center_y * scale_y_;
    }

    void calculate_dimensions() override {}
    
    std::unique_ptr<render::Element> render() const override;
    
    void handle_click(double relative_x, double relative_y) override {
        if (click_handler_) {
            auto [world_x, world_y] = zoom_pan_.screen_to_world(relative_x, relative_y);
            click_handler_(world_x, world_y);
        }
    }
    
    // Public methods for adding elements
    void add_point(double x, double y, const std::string& color, double radius, const std::string& label = "") {
        GeometryElement element(GeometryElement::Type::POINT);
        element.data = {x, y, radius};
        element.color = color;
        element.label = label;
        elements_.push_back(std::move(element));
        update_bounds(x, y);
    }
    
    void add_segment(double start_x, double start_y, double end_x, double end_y,
                     const std::string& color, double width, const std::string& style) {
        GeometryElement element(GeometryElement::Type::SEGMENT);
        element.data = {start_x, start_y, end_x, end_y, width};
        element.color = color;
        element.stroke_width = width;
        element.style = style;
        elements_.push_back(std::move(element));
        update_bounds(start_x, start_y);
        update_bounds(end_x, end_y);
    }
    
    void add_line(double x, double y, double dx, double dy,
                  const std::string& color, double width, const std::string& style) {
        GeometryElement element(GeometryElement::Type::LINE);
        element.data = {x, y, dx, dy, width};
        element.color = color;
        element.stroke_width = width;
        element.style = style;
        elements_.push_back(std::move(element));
        update_bounds(x, y);
    }
    
    void add_circle(double cx, double cy, double radius,
                    const std::string& color, const std::string& stroke_color,
                    double stroke_width, bool fill) {
        GeometryElement element(GeometryElement::Type::CIRCLE);
        element.data = {cx, cy, radius};
        element.color = color;
        element.stroke_color = stroke_color;
        element.stroke_width = stroke_width;
        element.fill = fill;
        elements_.push_back(std::move(element));
        update_bounds(cx - radius, cy - radius);
        update_bounds(cx + radius, cy + radius);
    }
    
    void add_polygon(const std::vector<double>& vertices,
                     const std::string& color, const std::string& stroke_color,
                     double stroke_width, bool fill) {
        GeometryElement element(GeometryElement::Type::POLYGON);
        element.data = vertices;
        for (size_t i = 0; i < vertices.size(); i += 2) {
            update_bounds(vertices[i], vertices[i+1]);
        }
        element.color = color;
        element.stroke_color = stroke_color;
        element.stroke_width = stroke_width;
        element.fill = fill;
        elements_.push_back(std::move(element));
    }
    
    void add_halfplane(double px, double py, double dx, double dy,
                       const std::string& color, const std::string& boundary_color) {
        GeometryElement element(GeometryElement::Type::HALFPLANE);
        element.data = {px, py, dx, dy};
        element.color = color;
        element.stroke_color = boundary_color;
        elements_.push_back(std::move(element));
    }
    
    void add_vector(double ox, double oy, double dx, double dy,
                    const std::string& color, double width, double head_size,
                    const std::string& label) {
        GeometryElement element(GeometryElement::Type::VECTOR);
        element.data = {ox, oy, dx, dy, width, head_size};
        element.color = color;
        element.stroke_width = width;
        element.label = label;
        elements_.push_back(std::move(element));
        update_bounds(ox, oy);
        update_bounds(ox + dx, oy + dy);
    }
    
    void highlight_point(double x, double y, bool highlight, const std::string& color) {
        for (auto& element : elements_) {
            if (element.type == GeometryElement::Type::POINT && 
                element.data.size() >= 2 &&
                std::abs(element.data[0] - x) < 1e-6 &&
                std::abs(element.data[1] - y) < 1e-6) {
                element.highlighted = highlight;
                if (highlight) {
                    element.color = color;
                }
                break;
            }
        }
    }
    
    void clear_highlights() {
        for (auto& element : elements_) {
            element.highlighted = false;
        }
    }
    
    void clear_all_elements() {
        elements_.clear();
        has_data_ = false;
        min_x_ = std::numeric_limits<double>::max();
        min_y_ = std::numeric_limits<double>::max();
        max_x_ = std::numeric_limits<double>::lowest();
        max_y_ = std::numeric_limits<double>::lowest();
    }
    
    void set_grid_enabled(bool enabled) { grid_enabled_ = enabled; }
    void set_grid_spacing(double spacing) { grid_spacing_ = spacing; }
    void set_grid_color(const std::string& color) { grid_color_ = color; }
    void set_axes_enabled(bool enabled) { axes_enabled_ = enabled; }
    void set_origin(double x, double y) { origin_x_ = x; origin_y_ = y; }
    void set_scale(double x, double y) { scale_x_ = x; scale_y_ = y; }
    void set_zoom_pan_enabled(bool enabled) { zoom_pan_.enabled = enabled; }
    void set_zoom_limits(double min_zoom, double max_zoom) { 
        zoom_pan_.min_zoom = min_zoom; 
        zoom_pan_.max_zoom = max_zoom; 
    }
    void set_click_handler(std::function<void(double, double)> handler) {
        click_handler_ = std::move(handler);
    }

protected:
    void render_grid(render::Group* group) const;
    void render_axes(render::Group* group) const;
    void render_element(render::Group* group, const GeometryElement& element) const;
};

} // namespace internal

// ============================================================================
// PUBLIC API IMPLEMENTATIONS
// ============================================================================

template<typename PointType>
void draw_point(const GeometryIDType& widget_id, const PointType& point, 
                const std::string& color, double radius,
                const std::string& label) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->add_point(point.x, point.y, color, radius, label);
    }
}

template<typename PointType>
void draw_points(const GeometryIDType& widget_id, const std::vector<PointType>& points,
                 const std::string& color, double radius,
                 const std::vector<std::string>& labels) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        for (size_t i = 0; i < points.size(); ++i) {
            widget->add_point(points[i].x, points[i].y, color, radius, 
                              i < labels.size() ? labels[i] : "");
        }
    }
}

template<typename PointType>
void draw_segment(const GeometryIDType& widget_id, const PointType& start, const PointType& end,
                  const std::string& color, double width,
                  const std::string& style) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->add_segment(start.x, start.y, end.x, end.y, color, width, style);
    }
}

template<typename PointType, typename VectorType>
void draw_line(const GeometryIDType& widget_id, const PointType& point, const VectorType& direction,
               const std::string& color, double width,
               const std::string& style) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->add_line(point.x, point.y, direction.x, direction.y, color, width, style);
    }
}

template<typename PointType>
void draw_circle(const GeometryIDType& widget_id, const PointType& center, double radius,
                 const std::string& color, const std::string& stroke_color,
                 double stroke_width, bool fill) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->add_circle(center.x, center.y, radius, color, stroke_color, stroke_width, fill);
    }
}

template<typename PointType>
void draw_polygon(const GeometryIDType& widget_id, const std::vector<PointType>& vertices,
                  const std::string& color, const std::string& stroke_color,
                  double stroke_width, bool fill) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        std::vector<double> points;
        for (const auto& v : vertices) {
            points.push_back(v.x);
            points.push_back(v.y);
        }
        widget->add_polygon(points, color, stroke_color, stroke_width, fill);
    }
}

template<typename PointType, typename VectorType>
void draw_halfplane(const GeometryIDType& widget_id, const PointType& boundary_point, 
                    const VectorType& direction, const std::string& color,
                    const std::string& boundary_color) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->add_halfplane(boundary_point.x, boundary_point.y, direction.x, direction.y, color, boundary_color);
    }
}

template<typename PointType, typename VectorType>
void draw_vector(const GeometryIDType& widget_id, const PointType& origin, const VectorType& direction,
                 const std::string& color, double width, double head_size,
                 const std::string& label) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->add_vector(origin.x, origin.y, direction.x, direction.y, color, width, head_size, label);
    }
}

template<typename PointType>
void highlight_point(const GeometryIDType& widget_id, const PointType& point, 
                     bool highlight, const std::string& color) {
    auto& state = internal::State::instance();
    if (auto* widget = state.get_widget_typed<internal::GeometryWidgetBase>(widget_id)) {
        widget->highlight_point(point.x, point.y, highlight, color);
    }
}

} // namespace edulcni

#endif // EDULCNI_WIDGETS_GEOMETRY_HPP 