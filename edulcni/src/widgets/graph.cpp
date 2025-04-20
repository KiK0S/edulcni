// This file intentionally left empty.
// All graph functions are implemented inline in
// include/edulcni/widgets/graph.hpp

#include "edulcni/widgets/graph.hpp"
#include <iostream>
#include <emscripten.h>

namespace edulcni {

void create_graph_widget(const std::string& id) {
    std::cout << "Create graph widget: " << id << std::endl;
}

void set_graph(const std::string& id, const std::vector<std::vector<int>>& graph) {
    std::cout << "Set graph: " << id << std::endl;
}

void highlight_vertex(const std::string& id, int vertex, bool highlight) {
    std::cout << "Highlight vertex: " << id << ", " << vertex << std::endl;
}

void highlight_edge(const std::string& id, int from, int to, bool highlight) {
    std::cout << "Highlight edge: " << id << ", " << from << " -> " << to << std::endl;
}

void clear_graph_highlights(const std::string& id) {
    std::cout << "Clear graph highlights: " << id << std::endl;
}

void graph_dimensions(const std::string& id) {
    std::cout << "Graph dimensions: " << id << std::endl;
}

} // namespace edulcni 