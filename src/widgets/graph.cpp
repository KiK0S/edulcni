#include "edulcni/widgets/graph.hpp"

namespace edulcni {

// Function to create a graph widget from an adjacency list
internal::GraphWidget* create_graph_widget(const std::string& id) {
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::GraphWidget>(id);
    return widget;
}

// Set the graph data from an adjacency list
void set_graph(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->set_graph(adj_list);
    }
}

// Set graph with specific strategy
void set_tree(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list, int root) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->set_tree(adj_list, root);
    }
}

void set_bfs_layered(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list, int start) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->set_bfs_layered(adj_list, start);
    }
}

void set_bipartite(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->set_bipartite(adj_list);
    }
}

void set_planar(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->set_planar(adj_list);
    }
}

// Highlight operations
void highlight_vertex(const std::string& graph_id, int vertex_id, bool highlight) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->highlight_vertex(vertex_id, highlight);
    }
}

void highlight_edge(const std::string& graph_id, int from, int to, bool highlight) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->highlight_edge(from, to, highlight);
    }
}

void clear_graph_highlights(const std::string& graph_id) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->clear_highlights();
    }
}

// Get widget dimensions
std::pair<double, double> graph_dimensions(const std::string& graph_id) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        return {widget->width(), widget->height()};
    }
    return {0, 0};
}

} // namespace edulcni 