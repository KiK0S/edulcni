/**
 * Horizontal comparison widget for edulcni
 * Allows showing two arrays side by side for visual comparison
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"

namespace edulcni {
// Public API for compare_h widget
void compare_h_widget(const std::string& id, 
                     const std::string& left_widget_id, 
                     const std::string& right_widget_id) ;

void compare_h_set_spacing(const std::string& id, double spacing) ;

namespace internal {

class CompareHWidget : public Widget {
private:
    std::string left_widget_id_;
    std::string right_widget_id_;
    double spacing_ = 20.0; // Space between the two widgets
    
public:
    CompareHWidget(std::string id) : Widget(std::move(id)) {}
    
    void set_widgets(const std::string& left_id, const std::string& right_id) {
        left_widget_id_ = left_id;
        right_widget_id_ = right_id;
        
        // Mark contained widgets as not directly renderable in the frame
        Widget* left_widget = State::instance().get_widget(left_widget_id_);
        Widget* right_widget = State::instance().get_widget(right_widget_id_);
        
        if (left_widget) {
            left_widget->set_container_id(id());
        }
        
        if (right_widget) {
            right_widget->set_container_id(id());
        }
        
        calculate_dimensions();
    }
    
    void set_spacing(double spacing) {
        spacing_ = spacing;
        calculate_dimensions();
    }
    
    void calculate_dimensions() override {
        // Get the widgets from the state
        Widget* left_widget = State::instance().get_widget(left_widget_id_);
        Widget* right_widget = State::instance().get_widget(right_widget_id_);
        
        if (!left_widget || !right_widget) {
            width_ = 0;
            height_ = 0;
            return;
        }

        left_widget->calculate_dimensions();
        right_widget->calculate_dimensions();
        
        // Calculaddate total width and maximum height
        width_ = left_widget->width() + spacing_ + right_widget->width();
        height_ = std::max(left_widget->height(), right_widget->height());
    }
    
    // Generate rendering instructions
    std::unique_ptr<render::Element> render() const override {
        auto group = std::make_unique<render::Group>();
        
        // Get the widgets from the state
        Widget* left_widget = State::instance().get_widget(left_widget_id_);
        Widget* right_widget = State::instance().get_widget(right_widget_id_);
        
        if (!left_widget || !right_widget) {
            return group;
        }
        
        // Render left widget
        auto left_render = left_widget->render();
        group->add(std::move(left_render));
        
        // Render right widget (positioned to the right of the left widget)
        auto right_render = right_widget->render();
        right_render->to_right(left_widget->width() + spacing_);
        group->add(std::move(right_render));
        
        return group;
    }
    
    // When this container is destroyed, release its widgets
    ~CompareHWidget() {
        // Free any widgets that are contained by this one
        Widget* left_widget = State::instance().get_widget(left_widget_id_);
        Widget* right_widget = State::instance().get_widget(right_widget_id_);
        
        if (left_widget && left_widget->container_id() == id()) {
            left_widget->set_container_id("");
        }
        
        if (right_widget && right_widget->container_id() == id()) {
            right_widget->set_container_id("");
        }
    }
};

} // namespace internal

} // namespace edulcni
