#include <edulcni.hpp>
#include <numeric>
#include <cmath>
#include <vector>
#include <algorithm>

int main() {
    edulcni::initialize("out/data_structure_showcase");
    edulcni::reset_layout(edulcni::LayoutStrategy::VerticalStack);

    std::vector<int> base = {5, 1, 7, 3, 6, 4, 2, 9};

    // ------------------------------------------------------------------
    // Segment tree view
    // ------------------------------------------------------------------
    edulcni::SegmentTreeView segment_tree("segment_tree", base.size());

    const size_t offset = segment_tree.leaf_offset();
    std::vector<int> tree_values(segment_tree.node_count(), 0);
    for (size_t i = 0; i < base.size(); ++i) {
        tree_values[offset + i] = base[i];
    }
    for (int idx = static_cast<int>(offset) - 1; idx >= 0; --idx) {
        tree_values[idx] = tree_values[2 * idx + 1] + tree_values[2 * idx + 2];
    }
    for (size_t i = 0; i < tree_values.size(); ++i) {
        segment_tree.set_value(i, std::to_string(tree_values[i]));
    }

    edulcni::step();

    segment_tree.highlight_range(2, 5);
    edulcni::step();
    segment_tree.clear_highlights();

    // ------------------------------------------------------------------
    // Sparse table view (range minimum queries)
    // ------------------------------------------------------------------
    size_t n = base.size();
    size_t levels = static_cast<size_t>(std::floor(std::log2(n))) + 1;
    std::vector<std::vector<int>> sparse_values(levels, std::vector<int>(n, 0));
    std::vector<std::vector<bool>> valid(levels, std::vector<bool>(n, false));
    std::vector<std::vector<std::pair<int, int>>> sparse_ranges(levels, std::vector<std::pair<int, int>>(n, {0, -1}));

    for (size_t i = 0; i < n; ++i) {
        sparse_values[0][i] = base[i];
        valid[0][i] = true;
        sparse_ranges[0][i] = {static_cast<int>(i), static_cast<int>(i)};
    }

    for (size_t k = 1; k < levels; ++k) {
        size_t len = 1u << k;
        for (size_t i = 0; i + len <= n; ++i) {
            sparse_values[k][i] = std::min(
                sparse_values[k - 1][i],
                sparse_values[k - 1][i + (len >> 1)]
            );
            valid[k][i] = true;
            sparse_ranges[k][i] = {
                static_cast<int>(i),
                static_cast<int>(i + len - 1)
            };
        }
    }

    std::vector<std::vector<std::string>> sparse_display(levels, std::vector<std::string>(n, ""));
    for (size_t k = 0; k < levels; ++k) {
        for (size_t i = 0; i < n; ++i) {
            if (valid[k][i]) {
                sparse_display[k][i] = std::to_string(sparse_values[k][i]);
            }
        }
    }

    edulcni::sparse_table_widget("sparse_table", sparse_display, sparse_ranges);
    edulcni::step();

    edulcni::matrix_highlight("sparse_table", 2, 1);
    edulcni::step();
    edulcni::matrix_clear_highlights("sparse_table");

    // ------------------------------------------------------------------
    // Treap view
    // ------------------------------------------------------------------
    edulcni::TreapView treap("treap_structure");
    std::vector<edulcni::TreapView::Node> treap_nodes = {
        {8, "8", "30", 3, 10, edulcni::render::Color(129, 199, 132)},
        {3, "3", "45", 1, 6, edulcni::render::Color(144, 202, 249)},
        {10, "10", "20", -1, 14, edulcni::render::Color(255, 224, 130)},
        {1, "1", "80", -1, -1, edulcni::render::Color(244, 143, 177)},
        {6, "6", "60", -1, 7, edulcni::render::Color(206, 147, 216)},
        {14, "14", "10", 13, -1, edulcni::render::Color(255, 205, 210)},
        {7, "7", "50", -1, -1, edulcni::render::Color(197, 202, 233)},
        {13, "13", "15", -1, -1, edulcni::render::Color(255, 171, 145)}
    };
    treap.set_tree(treap_nodes, 8);
    treap.highlight(6);
    treap.highlight(7);
    edulcni::step();
    treap.clear_highlights();

    // ------------------------------------------------------------------
    // Sqrt decomposition view
    // ------------------------------------------------------------------
    edulcni::SqrtDecompositionView sqrt_view("sqrt_decomposition");
    sqrt_view.set_array(base);

    size_t block_size = static_cast<size_t>(std::ceil(std::sqrt(static_cast<double>(base.size()))));
    std::vector<int> block_values;
    for (size_t i = 0; i < base.size(); i += block_size) {
        int block_sum = 0;
        for (size_t j = i; j < std::min(base.size(), i + block_size); ++j) {
            block_sum += base[j];
        }
        block_values.push_back(block_sum);
    }
    sqrt_view.set_blocks(block_values);

    sqrt_view.highlight_block(1);
    sqrt_view.highlight_element(3);
    edulcni::step();
    sqrt_view.clear_highlights();

    // ------------------------------------------------------------------
    // Line container (Convex Hull Trick) view
    // ------------------------------------------------------------------
    edulcni::LineContainerView line_view("line_container");
    line_view.set_domain(0.0, 10.0);
    line_view.set_lines({
        {2.0, 3.0, "2x + 3", edulcni::render::Color(52, 152, 219), false},
        {1.0, 8.0, "x + 8", edulcni::render::Color(46, 204, 113), false},
        {-0.5, 20.0, "-0.5x + 20", edulcni::render::Color(231, 76, 60), false}
    });
    line_view.set_queries({
        {2.0, 0, edulcni::render::Color(155, 89, 182)},
        {5.0, 2, edulcni::render::Color(241, 196, 15)}
    });

    line_view.highlight_line(0, true);
    edulcni::step();

    line_view.highlight_line(0, false);
    line_view.highlight_line(2, true);
    line_view.set_queries({
        {7.0, 2, edulcni::render::Color(39, 174, 96)}
    });
    edulcni::step();

    return 0;
}

