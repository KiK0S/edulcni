/**
 * Array widget for edulcni
 */
#pragma once

#include <vector>
#include <unordered_set>
#include <type_traits>
#include <algorithm>
#include <sstream>
#include <string>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"

namespace edulcni {
namespace internal {

class LabelWidget : public Widget {
  
public:
    LabelWidget(std::string id) : Widget(std::move(id)) {}
    const static constexpr double CELL_WIDTH = 60;
    const static constexpr double CELL_HEIGHT = 40;
    // Calculate dimensions based on array size
    void calculate_dimensions() override {
        width_ = CELL_WIDTH;
        height_ = CELL_HEIGHT;
    }

    std::string value_;

    // Generate rendering instructions for the array
    std::unique_ptr<render::Element> render() const override {
        
        // Create a group to hold all elements
        auto group = std::make_unique<render::Group>();
        
        group->add(std::make_unique<render::Rectangle>(
            0, 0, CELL_WIDTH, CELL_HEIGHT, 
            render::Color::White(), render::Color::Black(), 1.0
        ));
        
        // Cell value
        group->add(std::make_unique<render::Text>(
            CELL_WIDTH / 2, CELL_HEIGHT / 2, 
            id_ + ": " + value_, "14px Arial", render::Color::Black(), "center"
        ));
        return group;
    }

    void update(std::string label) {
        value_ = label;
    }
};

} // namespace internal

// Type deduction function to create the right widget type
template<typename T>
void label_widget(const std::string& id, T val) {
   
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::LabelWidget>(id);
    widget->update(edulcni::internal::to_string_any(val));
}

} // namespace edulcni 