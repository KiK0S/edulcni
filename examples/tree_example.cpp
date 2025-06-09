/**
 * Example of graph visualization with DFS algorithm
 */
#include <iostream>
#include <vector>
#include <unordered_set>
#include <stack>
#include "edulcni.hpp"

// Implement DFS with visualization
std::vector<bool> visited;

void dfs(const std::vector<std::vector<int>>& graph, int current, int parent = -1) {
    edulcni::highlight_vertex("tree", current);
    edulcni::step();
    visited[current] = true;
    for (auto to : graph[current]) {
        if (to == parent) {
            continue;
        }
        edulcni::highlight_edge("tree", current, to);
        edulcni::step();
        dfs(graph, to, current);
        edulcni::highlight_edge("tree", current, to, false);
        edulcni::step();
    }
}

int main() {
    // Initialize edulcni
    edulcni::initialize("./tree_viz_output");
    
    // Create a graph using adjacency list
    // vertices are numbered 0-5 (representing A-F in the original example)
    std::vector<std::vector<int>> graph = {
        {1, 2},
        {3, 4, 5},
        {},
        {},
        {6, 7},
        {},
        {},
        {}
    };
    
    // Create and display the graph
    edulcni::create_tree_widget("tree");
    edulcni::set_tree("tree", graph);
    edulcni::step();
    
    visited.assign(graph.size(), false);
    // Run DFS from vertex 0 (equivalent to 'A' in the original example)
    dfs(graph, 0);
    
    // Clear all highlights to show final state
    edulcni::clear_graph_highlights("tree");
    edulcni::step();
    
    std::cout << "Visualization exported to ./tree_viz_output\n";
    std::cout << "Open ./tree_viz_output/index.html in a browser to view\n";
    
    return 0;
} 