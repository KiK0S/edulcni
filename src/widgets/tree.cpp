#include "edulcni/widgets/tree.hpp"

namespace edulcni {

// Function to create a graph widget from an adjacency list
internal::TreeWidget* create_tree_widget(const std::string& id) {
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::TreeWidget>(id);
    return widget;
}

// Set the graph data from an adjacency list
void set_tree(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list, int root) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::TreeWidgetBase>(graph_id);
    if (widget) {
        widget->set_tree(adj_list, root);
    }
}

} // namespace edulcni 