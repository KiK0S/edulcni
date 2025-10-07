/**
 * Widget base class for edulcni
 */
#pragma once

#include <string>
#include <memory>
#include "edulcni/core/render.hpp"

namespace edulcni {
namespace internal {

class Widget {
protected:
    std::string id_;
    double width_ = 0;  // Widget width
    double height_ = 0; // Widget height
    double x_ = 0;      // Widget x position on canvas
    double y_ = 0;      // Widget y position on canvas
    std::string container_id_; // ID of container widget that includes this widget, if any
    
public:
    Widget(std::string id) : id_(std::move(id)) {}
    virtual ~Widget() = default;
    
    const std::string& id() const { return id_; }
    
    // Dimension accessors
    virtual double width() const { return width_; }
    virtual double height() const { return height_; }
    
    // Position accessors
    virtual double x() const { return x_; }
    virtual double y() const { return y_; }
    virtual void set_position(double x, double y) { x_ = x; y_ = y; }
    
    // Calculate dimensions based on content (to be implemented by derived classes)
    virtual void calculate_dimensions() = 0;
    
    // Generate rendering instructions
    virtual std::unique_ptr<render::Element> render() const = 0;
    
    // Handle click events (override in derived classes)
    virtual void handle_click(double relative_x, double relative_y) {}
    
    // Check if a point is within widget bounds
    virtual bool contains_point(double x, double y) const {
        double relative_x = x - x_;
        double relative_y = y - y_;
        return relative_x >= 0 && relative_x <= width_ && 
               relative_y >= 0 && relative_y <= height_;
    }
    
    // Set the container ID for this widget (empty string if not contained)
    void set_container_id(const std::string& container_id) {
        container_id_ = container_id;
    }
    
    // Get the container ID for this widget
    const std::string& container_id() const {
        return container_id_;
    }
    
    // Check if this widget is contained by another widget
    bool is_contained() const {
        return !container_id_.empty();
    }
};

} // namespace internal

// Click handling utility functions
void handle_widget_click(const std::string& widget_id, double x, double y);
void register_widget_click_handler(const std::string& widget_id);

#define DEFINE_WIDGET_TYPE(name) \
    struct name##IDType { \
        std::string value; \
        name##IDType() = default; \
        name##IDType(const char* s) : value(s) {} \
        name##IDType(const std::string& s) : value(s) {} \
        name##IDType(std::string&& s) : value(std::move(s)) {} \
        operator const std::string&() const { return value; } \
        operator std::string&() { return value; } \
        bool operator==(const name##IDType& other) const { return value == other.value; } \
        bool operator!=(const name##IDType& other) const { return value != other.value; } \
        bool operator<(const name##IDType& other) const { return value < other.value; } \
    };

} // namespace edulcni 