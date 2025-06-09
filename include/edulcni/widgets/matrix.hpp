/**
 * Matrix widget for edulcni
 */
#pragma once

#include <vector>
#include <unordered_set>
#include <type_traits>
#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"
#include "edulcni/utils/to_string.hpp"

namespace edulcni {
namespace internal {


// Type-erased base matrix widget
class MatrixWidgetBase : public Widget {
protected:
    // Hash function for pair
    struct PairHash {
        template <class T1, class T2>
        std::size_t operator() (const std::pair<T1, T2>& pair) const {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };

    std::unordered_set<std::pair<int, int>, PairHash> highlights_;
    size_t rows_ = 0;
    size_t cols_ = 0;
    
    // Constants for layout
    static constexpr double CELL_WIDTH = 60;
    static constexpr double CELL_HEIGHT = 40;
    static constexpr double PADDING = 5;
    
    
public:
    MatrixWidgetBase(std::string id) : Widget(std::move(id)) {}
    
    void highlight(int row, int col) {
        highlights_.insert(std::make_pair(row, col));
    }
    
    void clear_highlights() {
        highlights_.clear();
    }
    
    virtual size_t num_rows() const = 0;
    virtual size_t num_cols() const = 0;
    virtual std::vector<std::vector<std::string>> get_display_values() const = 0;
    
    // Calculate dimensions based on matrix size
    void calculate_dimensions() override {
        rows_ = num_rows();
        cols_ = num_cols();
        width_ = cols_ > 0 ? cols_ * (CELL_WIDTH + PADDING) - PADDING : CELL_WIDTH;
        height_ = rows_ > 0 ? rows_ * (CELL_HEIGHT + PADDING) - PADDING : CELL_HEIGHT;
    }
    
    // Generate rendering instructions for the matrix
    std::unique_ptr<render::Element> render() const override {
        // Get string representation of values
        std::vector<std::vector<std::string>> values = get_display_values();
        
        // Create a group to hold all elements
        auto group = std::make_unique<render::Group>();
        
        // Render each cell
        for (size_t i = 0; i < values.size(); i++) {
            for (size_t j = 0; j < values[i].size(); j++) {
                double x = j * (CELL_WIDTH + PADDING);
                double y = i * (CELL_HEIGHT + PADDING);
                
                // Create a cell group
                auto cell_group = std::make_unique<render::Group>(x, y);
                
                // Cell background
                render::Color bgColor = render::Color::White();
                if (highlights_.find(std::make_pair(i, j)) != highlights_.end()) {
                    bgColor = render::Color::Highlight();
                }
                
                cell_group->add(std::make_unique<render::Rectangle>(
                    0, 0, CELL_WIDTH, CELL_HEIGHT, 
                    bgColor, render::Color::Black(), 1.0
                ));
                
                // Cell value
                cell_group->add(std::make_unique<render::Text>(
                    CELL_WIDTH / 2, CELL_HEIGHT / 2, 
                    values[i][j], "14px Arial", render::Color::Black(), "center"
                ));
                
                // Add cell to main group
                group->add(std::move(cell_group));
            }
        }
        
        return group;
    }
};

// Typed matrix widget implementation
template<typename T>
class MatrixWidget : public MatrixWidgetBase {
private:
    std::vector<std::vector<T>> data_;
    
public:
    MatrixWidget(std::string id) : MatrixWidgetBase(std::move(id)) {}
    
    void update(const std::vector<std::vector<T>>& matrix) {
        data_ = matrix;
        clear_highlights();
        calculate_dimensions();
    }
    
    size_t num_rows() const override {
        return data_.size();
    }
    
    size_t num_cols() const override {
        return data_.empty() ? 0 : data_[0].size();
    }
    
    std::vector<std::vector<std::string>> get_display_values() const override {
        std::vector<std::vector<std::string>> result;
        for (const auto& row : data_) {
            std::vector<std::string> row_result;
            for (const auto& item : row) {
                row_result.push_back(to_string_any(item));
            }
            result.push_back(std::move(row_result));
        }
        return result;
    }
};

} // namespace internal

// Function to create and update a matrix widget
template<typename T>
void matrix_widget(const std::string& id, const std::vector<std::vector<T>>& matrix) {
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::MatrixWidget<T>>(id);
    widget->update(matrix);
}

// Common operations that work with any matrix widget type
void matrix_highlight(const std::string& id, int row, int col);
void matrix_clear_highlights(const std::string& id);

// Get widget dimensions
std::pair<double, double> matrix_dimensions(const std::string& id);

} // namespace edulcni
