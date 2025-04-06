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

namespace edulcni {
namespace internal {

// Helper function to convert any type to string for visualization
template <typename T>
std::string to_string_any(const T& value) {
    if constexpr (std::is_same_v<T, std::string>) {
        return "\"" + value + "\"";
    } else {
        std::ostringstream ss;
        ss << value;
        return ss.str();
    }
}

// Type-erased base array widget
class ArrayWidgetBase : public Widget {
protected:
    std::unordered_set<int> highlights_;
    
public:
    ArrayWidgetBase(std::string id) : Widget(std::move(id)) {}
    
    void highlight(int index) {
        highlights_.insert(index);
    }
    
    void clear_highlights() {
        highlights_.clear();
    }
    
    // Type-specific methods to be implemented by derived classes
    virtual json::value values_to_json() const = 0;
    virtual size_t size() const = 0;
    
    json::value to_json() const override {
        json::value result = json::object{};
        result["type"] = "array";
        
        // Get values from derived class
        result["values"] = values_to_json();
        
        json::value highlights = json::array{};
        for (int index : highlights_) {
            highlights.as_array().push_back(index);
        }
        result["highlights"] = std::move(highlights);
        
        return result;
    }
    
    void update_from_json(const json::value& data) override {
        // Not implemented for now
    }
    
    std::string widget_type() const override {
        return "array";
    }
};

// Typed array widget implementation
template<typename T>
class ArrayWidget : public ArrayWidgetBase {
private:
    std::vector<T> data_;
    
public:
    ArrayWidget(std::string id) : ArrayWidgetBase(std::move(id)) {}
    
    template<typename Iterator>
    void update(Iterator begin, Iterator end) {
        data_.clear();
        data_.insert(data_.end(), begin, end);
        clear_highlights();
    }
    
    json::value values_to_json() const override {
        json::value values = json::array{};
        for (const auto& item : data_) {
            values.as_array().push_back(to_string_any(item));
        }
        return values;
    }
    
    size_t size() const override {
        return data_.size();
    }
};

} // namespace internal

// Type deduction function to create the right widget type
template<typename Iterator>
void array_widget(const std::string& id, Iterator begin, Iterator end) {
    using ValueType = typename std::iterator_traits<Iterator>::value_type;
    
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::ArrayWidget<ValueType>>(id);
    widget->update(begin, end);
    internal::State::instance().update_widget(id);
}

// Overload for C-style arrays
template<typename T, size_t N>
void array_widget(const std::string& id, const T (&arr)[N]) {
    array_widget(id, std::begin(arr), std::end(arr));
}

// Common operations that work with any array widget type
void array_highlight(const std::string& id, int index) {
    auto* widget = internal::State::instance()
        .get_widget_base<internal::ArrayWidgetBase>(id);
    if (widget) {
        widget->highlight(index);
        internal::State::instance().update_widget(id);
    }
}

void array_clear_highlights(const std::string& id) {
    auto* widget = internal::State::instance()
        .get_widget_base<internal::ArrayWidgetBase>(id);
    if (widget) {
        widget->clear_highlights();
        internal::State::instance().update_widget(id);
    }
}

} // namespace edulcni 