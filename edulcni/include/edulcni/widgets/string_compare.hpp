/**
 * String comparison widget for edulcni
 */
#pragma once

#include <string>
#include <vector>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"

namespace edulcni {
namespace internal {

class StringCompareWidget : public Widget {
private:
    std::string str1_;
    std::string str2_;
    int pos1_ = -1;  // Highlighted position in first string
    int pos2_ = -1;  // Highlighted position in second string
    std::string result_; // Comparison result
    
public:
    StringCompareWidget(std::string id) : Widget(std::move(id)) {}
    
    void update(const std::string& str1, const std::string& str2) {
        str1_ = str1;
        str2_ = str2;
        pos1_ = -1;
        pos2_ = -1;
        result_ = "";
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
    
    json::value to_json() const override {
        json::value result = json::object{};
        result["type"] = "string_compare";
        result["str1"] = str1_;
        result["str2"] = str2_;
        result["pos1"] = pos1_;
        result["pos2"] = pos2_;
        result["result"] = result_;
        
        return result;
    }
    
    void update_from_json(const json::value& data) override {
        // Not implemented for now
    }
    
    std::string widget_type() const override {
        return "string_compare";
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

} // namespace edulcni 