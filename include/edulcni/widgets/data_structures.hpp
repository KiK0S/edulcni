/**
 * Data structure helper widgets for edulcni
 */
#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <cstddef>

#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"
#include "edulcni/widgets/graph.hpp"
#include "edulcni/widgets/array.hpp"
#include "edulcni/widgets/matrix.hpp"
#include "edulcni/utils/to_string.hpp"

namespace edulcni {

using LayoutStrategy = internal::LayoutStrategy;

// -----------------------------------------------------------------------------
// Sparse table helper
// -----------------------------------------------------------------------------

template <typename T>
void sparse_table_widget(const std::string& id,
                         const std::vector<std::vector<T>>& table,
                         const std::vector<std::vector<std::pair<int, int>>>& intervals = {}) {
    std::vector<std::vector<std::string>> display(table.size());
    for (size_t i = 0; i < table.size(); ++i) {
        display[i].resize(table[i].size());
        for (size_t j = 0; j < table[i].size(); ++j) {
            std::string value_str = internal::to_string_any(table[i][j]);
            if (i < intervals.size() && j < intervals[i].size()) {
                const auto& range = intervals[i][j];
                if (range.first <= range.second) {
                    value_str = "[" + std::to_string(range.first) + "," + std::to_string(range.second) + "] " + value_str;
                }
            }
            display[i][j] = std::move(value_str);
        }
    }

    matrix_widget(id, display);
}

// -----------------------------------------------------------------------------
// Segment tree visualizer
// -----------------------------------------------------------------------------

class SegmentTreeView {
public:
    SegmentTreeView(const std::string& id, size_t leaf_count);

    void set_value(size_t node_index, const std::string& value);
    void set_leaf_value(size_t leaf_index, const std::string& value);
    void clear_values();

    void highlight_node(size_t node_index, bool highlight = true);
    void highlight_range(size_t left, size_t right, bool highlight = true);
    void clear_highlights();

    size_t node_count() const { return values_.size(); }
    size_t leaf_offset() const { return base_size_ > 0 ? base_size_ - 1 : 0; }

private:
    std::string id_;
    internal::GraphWidgetBase* widget_ = nullptr;
    std::vector<std::string> values_;
    std::vector<std::pair<size_t, size_t>> ranges_;
    std::vector<bool> active_;
    size_t leaf_count_ = 0;
    size_t base_size_ = 1;

    void build_structure();
    void compute_ranges(size_t index, size_t left, size_t right);
    void update_labels();
};

// -----------------------------------------------------------------------------
// Treap visualizer
// -----------------------------------------------------------------------------

class TreapView {
public:
    struct Node {
        int id;
        std::string key;
        std::string priority;
        int left = -1;
        int right = -1;
        render::Color color = render::Color(200, 200, 255);
        bool highlighted = false;
    };

    TreapView(const std::string& id);

    void set_tree(const std::vector<Node>& nodes, int root_id);
    void highlight(int node_id, bool highlight = true);
    void clear_highlights();

private:
    std::string id_;
    internal::GraphWidgetBase* widget_ = nullptr;
    std::unordered_map<int, size_t> id_to_index_;
};

// -----------------------------------------------------------------------------
// Sqrt decomposition helper
// -----------------------------------------------------------------------------

class SqrtDecompositionView {
public:
    explicit SqrtDecompositionView(const std::string& id);

    template <typename T>
    void set_array(const std::vector<T>& values) {
        vector_widget(array_id_, values);
    }

    template <typename T>
    void set_blocks(const std::vector<T>& block_values) {
        vector_widget(blocks_id_, block_values);
    }

    void highlight_element(size_t index);
    void highlight_block(size_t index);
    void clear_highlights();

private:
    std::string array_id_;
    std::string blocks_id_;
};

// -----------------------------------------------------------------------------
// Line container helper
// -----------------------------------------------------------------------------

namespace internal {
    class LineContainerWidget;
}

class LineContainerView {
public:
    struct LineDefinition {
        double slope;
        double intercept;
        std::string label;
        render::Color color = render::Color(52, 152, 219);
        bool highlighted = false;
    };

    struct QueryPoint {
        double x;
        size_t line_index;
        render::Color color = render::Color::Red();
    };

    explicit LineContainerView(const std::string& id);

    void set_domain(double min_x, double max_x);
    void set_lines(const std::vector<LineDefinition>& lines);
    void set_queries(const std::vector<QueryPoint>& queries);
    void highlight_line(size_t index, bool highlight = true);
    void clear_highlights();

private:
    internal::LineContainerWidget* widget_ = nullptr;
};

// -----------------------------------------------------------------------------
// Utility helpers
// -----------------------------------------------------------------------------

inline void reset_layout(LayoutStrategy strategy = LayoutStrategy::VerticalStack) {
    set_layout_strategy(strategy);
}

} // namespace edulcni

