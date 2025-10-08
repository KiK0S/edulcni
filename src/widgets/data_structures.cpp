#include "edulcni/widgets/data_structures.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>

namespace edulcni {

// -----------------------------------------------------------------------------
// Segment tree view implementation
// -----------------------------------------------------------------------------

SegmentTreeView::SegmentTreeView(const std::string& id, size_t leaf_count)
    : id_(id), leaf_count_(leaf_count) {
    widget_ = internal::State::instance()
        .get_or_create_widget<internal::GraphWidget>(id_);

    base_size_ = 1;
    while (base_size_ < std::max<size_t>(1, leaf_count_)) {
        base_size_ <<= 1;
    }

    const size_t node_count = base_size_ * 2 - 1;
    values_.assign(node_count, std::string{});
    ranges_.assign(node_count, {0, 0});
    active_.assign(node_count, false);

    build_structure();
}

void SegmentTreeView::build_structure() {
    const size_t node_count = values_.size();
    if (node_count == 0 || !widget_) {
        return;
    }

    std::vector<std::vector<int>> adj(node_count);
    for (size_t i = 0; i < node_count; ++i) {
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;
        if (left < node_count) {
            adj[i].push_back(static_cast<int>(left));
            adj[left].push_back(static_cast<int>(i));
        }
        if (right < node_count) {
            adj[i].push_back(static_cast<int>(right));
            adj[right].push_back(static_cast<int>(i));
        }
    }

    widget_->set_tree(adj, 0);
    compute_ranges(0, 0, base_size_ - 1);
    update_labels();
}

void SegmentTreeView::compute_ranges(size_t index, size_t left, size_t right) {
    if (index >= values_.size()) {
        return;
    }

    if (index >= base_size_ - 1) {
        if (leaf_count_ == 0 || left >= leaf_count_) {
            ranges_[index] = {left, left};
            active_[index] = false;
        } else {
            size_t clamped_right = std::min(right, leaf_count_ - 1);
            ranges_[index] = {left, clamped_right};
            active_[index] = true;
        }
        return;
    }

    size_t mid = (left + right) / 2;
    compute_ranges(2 * index + 1, left, mid);
    compute_ranges(2 * index + 2, mid + 1, right);

    const bool left_active = active_[2 * index + 1];
    const bool right_active = active_[2 * index + 2];

    if (left_active && right_active) {
        ranges_[index] = {
            ranges_[2 * index + 1].first,
            ranges_[2 * index + 2].second
        };
        active_[index] = true;
    } else if (left_active) {
        ranges_[index] = ranges_[2 * index + 1];
        active_[index] = true;
    } else if (right_active) {
        ranges_[index] = ranges_[2 * index + 2];
        active_[index] = true;
    } else {
        ranges_[index] = {left, left};
        active_[index] = false;
    }
}

void SegmentTreeView::update_labels() {
    if (!widget_) {
        return;
    }

    std::vector<std::string> labels(values_.size(), "∅");
    for (size_t i = 0; i < values_.size(); ++i) {
        if (!active_[i] || leaf_count_ == 0) {
            continue;
        }

        const auto& range = ranges_[i];
        std::string label = "[" + std::to_string(range.first) + "," + std::to_string(range.second) + "]";
        if (!values_[i].empty()) {
            label += ": " + values_[i];
        }
        labels[i] = std::move(label);
    }

    widget_->set_vertex_labels(labels);
}

void SegmentTreeView::set_value(size_t node_index, const std::string& value) {
    if (node_index >= values_.size()) {
        return;
    }
    values_[node_index] = value;
    update_labels();
}

void SegmentTreeView::set_leaf_value(size_t leaf_index, const std::string& value) {
    size_t offset = leaf_offset();
    if (leaf_index >= leaf_count_ || offset + leaf_index >= values_.size()) {
        return;
    }
    set_value(offset + leaf_index, value);
}

void SegmentTreeView::clear_values() {
    std::fill(values_.begin(), values_.end(), std::string{});
    update_labels();
}

void SegmentTreeView::highlight_node(size_t node_index, bool highlight) {
    if (!widget_ || node_index >= values_.size()) {
        return;
    }
    widget_->highlight_vertex(static_cast<int>(node_index), highlight);
}

void SegmentTreeView::highlight_range(size_t left, size_t right, bool highlight) {
    if (!widget_ || values_.empty() || left > right) {
        return;
    }

    for (size_t i = 0; i < values_.size(); ++i) {
        if (!active_[i]) {
            continue;
        }
        const auto& range = ranges_[i];
        if (range.first >= left && range.second <= right) {
            widget_->highlight_vertex(static_cast<int>(i), highlight);
        }
    }
}

void SegmentTreeView::clear_highlights() {
    if (widget_) {
        widget_->clear_highlights();
    }
}

// -----------------------------------------------------------------------------
// Treap view implementation
// -----------------------------------------------------------------------------

TreapView::TreapView(const std::string& id) : id_(id) {
    widget_ = internal::State::instance()
        .get_or_create_widget<internal::GraphWidget>(id_);
}

void TreapView::set_tree(const std::vector<Node>& nodes, int root_id) {
    if (!widget_) {
        return;
    }

    id_to_index_.clear();
    if (nodes.empty()) {
        widget_->set_graph({});
        return;
    }

    widget_->clear_highlights();

    for (size_t i = 0; i < nodes.size(); ++i) {
        id_to_index_[nodes[i].id] = i;
    }

    std::vector<std::vector<int>> adj(nodes.size());
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i].left != -1) {
            auto it = id_to_index_.find(nodes[i].left);
            if (it != id_to_index_.end()) {
                adj[i].push_back(static_cast<int>(it->second));
                adj[it->second].push_back(static_cast<int>(i));
            }
        }
        if (nodes[i].right != -1) {
            auto it = id_to_index_.find(nodes[i].right);
            if (it != id_to_index_.end()) {
                adj[i].push_back(static_cast<int>(it->second));
                adj[it->second].push_back(static_cast<int>(i));
            }
        }
    }

    size_t root_index = 0;
    if (auto it = id_to_index_.find(root_id); it != id_to_index_.end()) {
        root_index = it->second;
    }

    widget_->set_tree(adj, static_cast<int>(root_index));

    std::vector<std::string> labels(nodes.size());
    for (size_t i = 0; i < nodes.size(); ++i) {
        std::string label = nodes[i].key;
        if (!nodes[i].priority.empty()) {
            label += " | " + nodes[i].priority;
        }
        labels[i] = std::move(label);
        widget_->set_vertex_color(static_cast<int>(i), nodes[i].color);
        if (nodes[i].highlighted) {
            widget_->highlight_vertex(static_cast<int>(i), true);
        }
    }

    widget_->set_vertex_labels(labels);
}

void TreapView::highlight(int node_id, bool highlight) {
    if (!widget_) {
        return;
    }
    auto it = id_to_index_.find(node_id);
    if (it != id_to_index_.end()) {
        widget_->highlight_vertex(static_cast<int>(it->second), highlight);
    }
}

void TreapView::clear_highlights() {
    if (widget_) {
        widget_->clear_highlights();
    }
}

// -----------------------------------------------------------------------------
// Sqrt decomposition view implementation
// -----------------------------------------------------------------------------

SqrtDecompositionView::SqrtDecompositionView(const std::string& id)
    : array_id_(id + "_array"), blocks_id_(id + "_blocks") {}

void SqrtDecompositionView::highlight_element(size_t index) {
    highlight_single(array_id_, static_cast<int>(index));
}

void SqrtDecompositionView::highlight_block(size_t index) {
    highlight_single(blocks_id_, static_cast<int>(index));
}

void SqrtDecompositionView::clear_highlights() {
    ::edulcni::clear_highlights(array_id_);
    ::edulcni::clear_highlights(blocks_id_);
}

// -----------------------------------------------------------------------------
// Line container widget implementation
// -----------------------------------------------------------------------------

namespace internal {

namespace {
    std::string default_line_label(double slope, double intercept) {
        std::ostringstream ss;
        ss.setf(std::ios::fixed);
        ss.precision(2);
        ss << "y = " << slope << "x";
        if (intercept > 1e-9) {
            ss << " + " << intercept;
        } else if (intercept < -1e-9) {
            ss << " - " << std::abs(intercept);
        }
        return ss.str();
    }
}

class LineContainerWidget : public Widget {
public:
    struct LineState {
        double slope = 0.0;
        double intercept = 0.0;
        std::string label;
        render::Color color = render::Color(52, 152, 219);
        bool highlighted = false;
    };

    struct QueryState {
        double x = 0.0;
        size_t line_index = 0;
        render::Color color = render::Color::Red();
    };

    LineContainerWidget(std::string id) : Widget(std::move(id)) {
        calculate_dimensions();
    }

    void set_domain(double min_x, double max_x) {
        if (min_x == max_x) {
            max_x += 1.0;
        }
        min_x_ = std::min(min_x, max_x);
        max_x_ = std::max(min_x, max_x);
        update_range();
    }

    void set_lines(std::vector<LineState> lines) {
        lines_ = std::move(lines);
        update_range();
    }

    void set_queries(std::vector<QueryState> queries) {
        queries_ = std::move(queries);
        update_range();
    }

    void highlight_line(size_t index, bool highlight) {
        if (index < lines_.size()) {
            lines_[index].highlighted = highlight;
        }
    }

    void clear_highlights() {
        for (auto& line : lines_) {
            line.highlighted = false;
        }
    }

    void calculate_dimensions() override {
        width_ = 620;
        height_ = 380;
    }

    std::unique_ptr<render::Element> render() const override {
        auto group = std::make_unique<render::Group>();

        group->add(std::make_unique<render::Rectangle>(
            0, 0, width_, height_, render::Color(248, 249, 250), render::Color(224, 224, 224), 1.0
        ));

        const double padding = 50.0;
        const double plot_width = std::max(10.0, width_ - 2 * padding);
        const double plot_height = std::max(10.0, height_ - 2 * padding);

        auto map_x = [&](double x) {
            if (std::abs(max_x_ - min_x_) < 1e-9) {
                return padding + plot_width / 2.0;
            }
            return padding + (x - min_x_) / (max_x_ - min_x_) * plot_width;
        };

        auto map_y = [&](double y) {
            if (std::abs(max_y_ - min_y_) < 1e-9) {
                return padding + plot_height / 2.0;
            }
            double normalized = (y - min_y_) / (max_y_ - min_y_);
            return padding + plot_height - normalized * plot_height;
        };

        // Plot background
        group->add(std::make_unique<render::Rectangle>(
            padding, padding, plot_width, plot_height,
            render::Color::White(), render::Color(210, 210, 210), 1.0
        ));

        // Axes
        if (min_x_ <= 0.0 && max_x_ >= 0.0) {
            double axis_x = map_x(0.0);
            group->add(std::make_unique<render::Line>(
                axis_x, padding, axis_x, padding + plot_height,
                render::Color(200, 200, 200), 1.0
            ));
        }
        if (min_y_ <= 0.0 && max_y_ >= 0.0) {
            double axis_y = map_y(0.0);
            group->add(std::make_unique<render::Line>(
                padding, axis_y, padding + plot_width, axis_y,
                render::Color(200, 200, 200), 1.0
            ));
        }

        for (size_t idx = 0; idx < lines_.size(); ++idx) {
            const auto& line = lines_[idx];
            double y1 = line.slope * min_x_ + line.intercept;
            double y2 = line.slope * max_x_ + line.intercept;

            double sx1 = map_x(min_x_);
            double sy1 = map_y(y1);
            double sx2 = map_x(max_x_);
            double sy2 = map_y(y2);

            render::Color color = line.highlighted ? render::Color::Red() : line.color;
            double stroke_width = line.highlighted ? 3.0 : 1.5;

            group->add(std::make_unique<render::Line>(
                sx1, sy1, sx2, sy2, color, stroke_width
            ));

            std::string label = line.label.empty() ? default_line_label(line.slope, line.intercept) : line.label;
            group->add(std::make_unique<render::Text>(
                sx2 - 6, sy2 - 12, label, "12px Arial", color, "right"
            ));
        }

        for (const auto& query : queries_) {
            if (query.line_index >= lines_.size()) {
                continue;
            }
            const auto& line = lines_[query.line_index];
            double y = line.slope * query.x + line.intercept;

            double sx = map_x(query.x);
            double sy = map_y(y);

            group->add(std::make_unique<render::Circle>(
                sx, sy, 5.0, query.color, render::Color::Black(), 1.0
            ));

            std::ostringstream label;
            label.setf(std::ios::fixed);
            label.precision(2);
            label << "x=" << query.x << " → " << y;
            group->add(std::make_unique<render::Text>(
                sx + 8, sy - 8, label.str(), "11px Arial", query.color, "left"
            ));
        }

        return group;
    }

private:
    double min_x_ = -10.0;
    double max_x_ = 10.0;
    double min_y_ = -10.0;
    double max_y_ = 10.0;
    std::vector<LineState> lines_;
    std::vector<QueryState> queries_;

    void update_range() {
        double min_val = std::numeric_limits<double>::max();
        double max_val = std::numeric_limits<double>::lowest();

        auto consider = [&](double y) {
            min_val = std::min(min_val, y);
            max_val = std::max(max_val, y);
        };

        for (const auto& line : lines_) {
            consider(line.slope * min_x_ + line.intercept);
            consider(line.slope * max_x_ + line.intercept);
        }
        for (const auto& query : queries_) {
            if (query.line_index < lines_.size()) {
                const auto& line = lines_[query.line_index];
                consider(line.slope * query.x + line.intercept);
            }
        }

        if (min_val == std::numeric_limits<double>::max()) {
            min_val = -1.0;
            max_val = 1.0;
        }

        double padding = (max_val - min_val) * 0.1;
        if (padding < 1.0) {
            padding = 1.0;
        }

        min_y_ = min_val - padding;
        max_y_ = max_val + padding;
        if (std::abs(max_y_ - min_y_) < 1e-9) {
            max_y_ = min_y_ + 1.0;
        }
    }
};

} // namespace internal

// -----------------------------------------------------------------------------
// Line container view implementation
// -----------------------------------------------------------------------------

LineContainerView::LineContainerView(const std::string& id) {
    widget_ = internal::State::instance()
        .get_or_create_widget<internal::LineContainerWidget>(id);
}

void LineContainerView::set_domain(double min_x, double max_x) {
    if (widget_) {
        widget_->set_domain(min_x, max_x);
    }
}

void LineContainerView::set_lines(const std::vector<LineDefinition>& lines) {
    if (!widget_) {
        return;
    }
    std::vector<internal::LineContainerWidget::LineState> converted;
    converted.reserve(lines.size());
    for (const auto& line : lines) {
        internal::LineContainerWidget::LineState state;
        state.slope = line.slope;
        state.intercept = line.intercept;
        state.label = line.label;
        state.color = line.color;
        state.highlighted = line.highlighted;
        converted.push_back(std::move(state));
    }
    widget_->set_lines(std::move(converted));
}

void LineContainerView::set_queries(const std::vector<QueryPoint>& queries) {
    if (!widget_) {
        return;
    }
    std::vector<internal::LineContainerWidget::QueryState> converted;
    converted.reserve(queries.size());
    for (const auto& query : queries) {
        internal::LineContainerWidget::QueryState state;
        state.x = query.x;
        state.line_index = query.line_index;
        state.color = query.color;
        converted.push_back(std::move(state));
    }
    widget_->set_queries(std::move(converted));
}

void LineContainerView::highlight_line(size_t index, bool highlight) {
    if (widget_) {
        widget_->highlight_line(index, highlight);
    }
}

void LineContainerView::clear_highlights() {
    if (widget_) {
        widget_->clear_highlights();
    }
}

} // namespace edulcni

