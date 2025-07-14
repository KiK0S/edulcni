#pragma once

#include <string>
#include <functional>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"

namespace edulcni {

// Legacy function for registering button click handlers
void register_button_click_handler(const std::string& id, 
                                 std::function<void()> callback);

// Function to create a button widget
void button_widget(const std::string& id, const std::string& text, 
                  std::function<void()> click_handler = nullptr);

// Function to trigger button click (for testing or programmatic clicks)
void button_click(const std::string& id);

namespace internal {

class ButtonWidget : public Widget {
private:
    std::string text_;
    std::function<void()> click_handler_;
    bool is_pressed_ = false;
    
    // Constants for layout
    static constexpr double MIN_WIDTH = 80;
    static constexpr double MIN_HEIGHT = 40;
    static constexpr double PADDING = 10;
    
public:
    ButtonWidget(std::string id) : Widget(std::move(id)) {}
    
    void set_text(const std::string& text) {
        text_ = text;
        calculate_dimensions();
    }
    
    void set_click_handler(std::function<void()> handler) {
        click_handler_ = handler;
    }
    
    void set_pressed(bool pressed) {
        is_pressed_ = pressed;
    }
    
    const std::string& text() const { return text_; }
    
    // Calculate dimensions based on text content
    void calculate_dimensions() override {
        // Rough calculation: 8 pixels per character plus padding
        double text_width = text_.length() * 8 + PADDING * 2;
        width_ = std::max(text_width, MIN_WIDTH);
        height_ = MIN_HEIGHT;
    }
    
    // Generate rendering instructions for the button
    std::unique_ptr<render::Element> render() const override {
        auto group = std::make_unique<render::Group>();
        
        // Button background (changes color when pressed)
        render::Color bg_color = is_pressed_ ? 
            render::Color::Highlight() : 
            render::Color(220, 220, 220); // Light gray for button
            
        group->add(std::make_unique<render::Rectangle>(
            0, 0, width_, height_,
            bg_color, render::Color::Black(), 2.0
        ));
        
        // Button text
        group->add(std::make_unique<render::Text>(
            width_ / 2, height_ / 2,
            text_, "14px Arial", render::Color::Black(), "center"
        ));
        
        return group;
    }
    
    // Override base class click handling
    void handle_click(double relative_x, double relative_y) override {
        if (click_handler_) {
            click_handler_();
        }
    }
    
    // Legacy method for backward compatibility
    void handle_click() {
        handle_click(0, 0);
    }
};

} // namespace internal

} // namespace edulcni 