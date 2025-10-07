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
#include "edulcni/utils/to_string.hpp"

namespace edulcni {

DEFINE_WIDGET_TYPE(Array)

template<typename Iterator>
ArrayIDType array_widget(const ArrayIDType& id, Iterator begin, Iterator end);
template<typename T>
ArrayIDType vector_widget(const ArrayIDType& id, const std::vector<T>& v);
template<typename T, size_t N>
ArrayIDType array_widget(const ArrayIDType& id, const T (&arr)[N]);
ArrayIDType array_widget(const ArrayIDType& id, std::initializer_list<int> v);
ArrayIDType string_widget(const ArrayIDType& id, const std::string& s);
ArrayIDType bigint_widget(const ArrayIDType& id, const std::string& s);
ArrayIDType bitint_widget(const ArrayIDType& id, int x);

// Common operations that work with any array widget type
void highlight_single(const ArrayIDType& id, int index);
void highlight_subsegment(const ArrayIDType& id, int start, int end);
template<typename Iterator>
void highlight_subsequence(const ArrayIDType& id, Iterator begin, Iterator end);
void highlight_subsequence(const ArrayIDType& id, std::initializer_list<int> indices);
void clear_highlights(const ArrayIDType& id);

// Get widget dimensions
std::pair<double, double> array_dimensions(const ArrayIDType& id);

namespace internal {

// Type-erased base array widget
class ArrayWidgetBase : public Widget {
protected:
    std::unordered_set<int> highlights_;
    
    // Constants for layout
    static constexpr double CELL_WIDTH = 60;
    static constexpr double CELL_HEIGHT = 40;
    static constexpr double PADDING = 5;
    
public:
    ArrayWidgetBase(std::string id) : Widget(std::move(id)) {}
    
    void highlight(int index) {
        highlights_.insert(index);
    }
    
    void clear_highlights() {
        highlights_.clear();
    }
    
    virtual size_t size() const = 0;
    virtual std::vector<std::string> get_display_values() const = 0;
    
    // Calculate dimensions based on array size
    void calculate_dimensions() override {
        size_t arr_size = size();
        width_ = arr_size > 0 ? arr_size * (CELL_WIDTH + PADDING) - PADDING : CELL_WIDTH;
        height_ = CELL_HEIGHT;
    }
    
    // Generate rendering instructions for the array
    std::unique_ptr<render::Element> render() const override {
        // Get string representation of values
        std::vector<std::string> values = get_display_values();
        
        // Create a group to hold all elements
        auto group = std::make_unique<render::Group>();
        
        // Render each cell
        for (size_t i = 0; i < values.size(); i++) {
            double x = i * (CELL_WIDTH + PADDING);
            double y = 0;
            
            // Create a cell group
            auto cell_group = std::make_unique<render::Group>(x, y);
            
            // Cell background
            render::Color bgColor = render::Color::White();
            if (highlights_.find(i) != highlights_.end()) {
                bgColor = render::Color::Highlight();
            }
            
            cell_group->add(std::make_unique<render::Rectangle>(
                0, 0, CELL_WIDTH, CELL_HEIGHT, 
                bgColor, render::Color::Black(), 1.0
            ));
            
            // Cell value
            cell_group->add(std::make_unique<render::Text>(
                CELL_WIDTH / 2, CELL_HEIGHT / 2, 
                values[i], "14px Arial", render::Color::Black(), "center"
            ));
            
            // Add cell to main group
            group->add(std::move(cell_group));
        }
        
        return group;
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
        calculate_dimensions();
    }
    
    size_t size() const override {
        return data_.size();
    }
    
    std::vector<std::string> get_display_values() const override {
        std::vector<std::string> result;
        for (const auto& item : data_) {
            result.push_back(to_string_any(item));
        }
        return result;
    }
};

} // namespace internal

// Type deduction function to create the right widget type
template<typename Iterator>
ArrayIDType array_widget(const ArrayIDType& id, Iterator begin, Iterator end) {
    using ValueType = typename std::iterator_traits<Iterator>::value_type;
    
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::ArrayWidget<ValueType>>(id);
    widget->update(begin, end);
    return id;
}


template<typename T>
ArrayIDType vector_widget(const ArrayIDType& id, const std::vector<T>& v) {
    return array_widget(id, v.begin(), v.end());
}

// Overload for C-style arrays
template<typename T, size_t N>
ArrayIDType array_widget(const ArrayIDType& id, const T (&arr)[N]) {
    return array_widget(id, std::begin(arr), std::end(arr));
}

} // namespace edulcni 