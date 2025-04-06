/**
 * Widget base class for edulcni
 */
#pragma once

#include <string>
#include <memory>
#include "edulcni/core/json.hpp"
#include "edulcni/core/render.hpp"

namespace edulcni {
namespace internal {

class Widget {
protected:
    std::string id_;
    double width_ = 0;  // Widget width
    double height_ = 0; // Widget height
    
public:
    Widget(std::string id) : id_(std::move(id)) {}
    virtual ~Widget() = default;
    
    const std::string& id() const { return id_; }
    
    // Dimension accessors
    virtual double width() const { return width_; }
    virtual double height() const { return height_; }
    
    // Calculate dimensions based on content (to be implemented by derived classes)
    virtual void calculate_dimensions() = 0;
    
    virtual json::value to_json() const = 0;
    virtual void update_from_json(const json::value& data) = 0;
    virtual std::string widget_type() const = 0;
    
    // Generate rendering instructions
    virtual std::unique_ptr<render::Element> render() const = 0;
};

} // namespace internal
} // namespace edulcni 