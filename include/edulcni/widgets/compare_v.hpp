/**
 * Vertical comparison widget for edulcni
 * Allows showing two arrays stacked vertically for comparison
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"

namespace edulcni {
namespace internal {

class CompareVWidget : public Widget {
private:
    std::string top_widget_id_;
    std::string bottom_widget_id_;
    double spacing_ = 20.0; // Space between the two widgets
    
    
public:
    CompareVWidget(std::string id) : Widget(std::move(id)) {}
    
    void set_widgets(const std::string& top_id, const std::string& bottom_id) {
        top_widget_id_ = top_id;
        bottom_widget_id_ = bottom_id;
        
        // Mark contained widgets as not directly renderable in the frame
        Widget* top_widget = State::instance().get_widget(top_widget_id_);
        Widget* bottom_widget = State::instance().get_widget(bottom_widget_id_);
        
        if (top_widget) {
            top_widget->set_container_id(id());
        }
        
        if (bottom_widget) {
            bottom_widget->set_container_id(id());
        }
        
        calculate_dimensions();
    }
    
    void set_spacing(double spacing) {
        spacing_ = spacing;
        calculate_dimensions();
    }

    void calculate_dimensions() override {
        // Get the widgets from the state
        Widget* top_widget = State::instance().get_widget(top_widget_id_);
        Widget* bottom_widget = State::instance().get_widget(bottom_widget_id_);
        
        if (!top_widget || !bottom_widget) {
            width_ = 0;
            height_ = 0;
            return;
        }

        top_widget->calculate_dimensions();
        bottom_widget->calculate_dimensions();
        
        // Calculate maximum width and total height
        width_ = std::max(top_widget->width(), bottom_widget->width());
        height_ = top_widget->height() + spacing_ + bottom_widget->height();
    }
    
    // Generate rendering instructions
    std::unique_ptr<render::Element> render() const override {
        auto group = std::make_unique<render::Group>();
        
        // Get the widgets from the state
        Widget* top_widget = State::instance().get_widget(top_widget_id_);
        Widget* bottom_widget = State::instance().get_widget(bottom_widget_id_);
        
        if (!top_widget || !bottom_widget) {
            return group;
        }
        
        // Render top widget
        auto top_render = top_widget->render();
        group->add(std::move(top_render));
        
        // Render bottom widget (positioned below the top widget)
        auto bottom_render = bottom_widget->render();
        bottom_render->to_bottom(top_widget->height() + spacing_);
        group->add(std::move(bottom_render));

        
        return group;
    }
    
    // When this container is destroyed, release its widgets
    ~CompareVWidget() {
        // Free any widgets that are contained by this one
        Widget* top_widget = State::instance().get_widget(top_widget_id_);
        Widget* bottom_widget = State::instance().get_widget(bottom_widget_id_);
        
        if (top_widget && top_widget->container_id() == id()) {
            top_widget->set_container_id("");
        }
        
        if (bottom_widget && bottom_widget->container_id() == id()) {
            bottom_widget->set_container_id("");
        }
    }
};

} // namespace internal

// Public API for compare_v widget
void compare_v_widget(const std::string& id, 
                     const std::string& top_widget_id, 
                     const std::string& bottom_widget_id) ;

void compare_v_set_spacing(const std::string& id, double spacing) ;
} // namespace edulcni 