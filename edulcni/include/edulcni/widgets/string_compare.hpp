/**
 * String comparison widget for edulcni
 */
#pragma once

#include <string>
#include <vector>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"

namespace edulcni {
namespace internal {

class StringCompareWidget : public Widget {
private:
    std::string str1_;
    std::string str2_;
    int pos1_ = -1;  // Highlighted position in first string
    int pos2_ = -1;  // Highlighted position in second string
    std::string result_; // Comparison result
    double width_ = 0;   // Widget width
    double height_ = 0;  // Widget height
    
    // Constants for layout
    static constexpr double CHAR_WIDTH = 30;
    static constexpr double CHAR_HEIGHT = 30;
    static constexpr double PADDING = 2;
    static constexpr double ROW_SPACING = 10;
    static constexpr double RESULT_HEIGHT = 30; // Fixed height for result text
    
public:
    StringCompareWidget(std::string id) : Widget(std::move(id)) {}
    
    void update(const std::string& str1, const std::string& str2) {
        str1_ = str1;
        str2_ = str2;
        pos1_ = -1;
        pos2_ = -1;
        result_ = "";
        calculate_dimensions();
    }
    
    void highlight(int pos1, int pos2, const std::string& result = "") {
        pos1_ = pos1;
        pos2_ = pos2;
        result_ = result;
    }
    
    void clear_highlight() {
        pos1_ = -1;
        pos2_ = -1;
        result_ = "";
    }
    
    // Calculate widget dimensions based on string lengths
    void calculate_dimensions() {
        size_t max_length = std::max(str1_.length(), str2_.length());
        width_ = max_length > 0 ? max_length * (CHAR_WIDTH + PADDING) : CHAR_WIDTH;
        // Always reserve space for result text + two strings
        height_ = RESULT_HEIGHT + (2 * CHAR_HEIGHT) + ROW_SPACING;
    }
    
    json::value to_json() const override {
        json::value result = json::object{};
        result["type"] = "string_compare";
        result["str1"] = str1_;
        result["str2"] = str2_;
        result["pos1"] = pos1_;
        result["pos2"] = pos2_;
        result["result"] = result_;
        result["width"] = width_;
        result["height"] = height_;
        
        return result;
    }
    
    void update_from_json(const json::value& data) override {
        // Not implemented for now
    }
    
    std::string widget_type() const override {
        return "string_compare";
    }
    
    // Get widget dimensions
    double width() const {
        return width_;
    }
    
    double height() const {
        return height_;
    }
    
    // Generate rendering instructions for string comparison
    std::unique_ptr<render::Element> render() const override {
        auto group = std::make_unique<render::Group>();
        
        // Add result text at the top (in the reserved space)
        group->add(std::make_unique<render::Text>(
            0, RESULT_HEIGHT/2, result_, "14px Arial", render::Color::Black(), "left"
        ));
        
        // Render first string
        for (size_t i = 0; i < str1_.length(); i++) {
            double x = i * (CHAR_WIDTH + PADDING);
            double y = RESULT_HEIGHT;
            
            render::Color bgColor = render::Color::White();
            if (static_cast<int>(i) == pos1_) {
                bgColor = render::Color::Highlight();
            }
            
            group->add(std::make_unique<render::Rectangle>(
                x, y, CHAR_WIDTH, CHAR_HEIGHT, 
                bgColor, render::Color::Black(), 1.0
            ));
            
            group->add(std::make_unique<render::Text>(
                x + CHAR_WIDTH/2, y + CHAR_HEIGHT/2, 
                std::string(1, str1_[i]), "14px Arial", render::Color::Black(), "center"
            ));
        }
        
        // Render second string
        for (size_t i = 0; i < str2_.length(); i++) {
            double x = i * (CHAR_WIDTH + PADDING);
            double y = RESULT_HEIGHT + CHAR_HEIGHT + ROW_SPACING;
            
            render::Color bgColor = render::Color::White();
            if (static_cast<int>(i) == pos2_) {
                bgColor = render::Color::Highlight();
            }
            
            group->add(std::make_unique<render::Rectangle>(
                x, y, CHAR_WIDTH, CHAR_HEIGHT, 
                bgColor, render::Color::Black(), 1.0
            ));
            
            group->add(std::make_unique<render::Text>(
                x + CHAR_WIDTH/2, y + CHAR_HEIGHT/2, 
                std::string(1, str2_[i]), "14px Arial", render::Color::Black(), "center"
            ));
        }
        
        return group;
    }
};

} // namespace internal

// Public API
void string_compare_widget(const std::string& id, const std::string& str1, const std::string& str2) {
    auto* widget = internal::State::instance().get_or_create_widget<internal::StringCompareWidget>(id);
    widget->update(str1, str2);
    internal::State::instance().update_widget(id);
}

void string_compare_highlight(const std::string& id, int pos1, int pos2, const std::string& result = "") {
    auto* widget = internal::State::instance().get_or_create_widget<internal::StringCompareWidget>(id);
    widget->highlight(pos1, pos2, result);
    internal::State::instance().update_widget(id);
}

void string_compare_clear(const std::string& id) {
    auto* widget = internal::State::instance().get_or_create_widget<internal::StringCompareWidget>(id);
    widget->clear_highlight();
    internal::State::instance().update_widget(id);
}

// Get widget dimensions
std::pair<double, double> string_compare_dimensions(const std::string& id) {
    auto* widget = internal::State::instance().get_or_create_widget<internal::StringCompareWidget>(id);
    return {widget->width(), widget->height()};
}

} // namespace edulcni 