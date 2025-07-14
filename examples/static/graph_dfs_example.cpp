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
    edulcni::highlight_vertex("graph", current);
    visited[current] = true;
    for (auto to : graph[current]) {
        if (to == parent) {
            continue;
        }
        edulcni::highlight_edge("graph", current, to);
        edulcni::step();
        if (visited[to]) {
            edulcni::highlight_edge("graph", current, to, false);
            edulcni::step();
            continue;
        }
        dfs(graph, to, current);
        edulcni::highlight_edge("graph", current, to, false);
        edulcni::step();
    }
}

int main() {
    // Initialize edulcni
    edulcni::initialize("./graph_viz_output");
    
    // Create a graph using adjacency list
    // vertices are numbered 0-5 (representing A-F in the original example)
    std::vector<std::vector<int>> graph = {
        {1, 4},    // 0 (A) connected to 1 (B) and 4 (E)
        {0, 2, 4}, // 1 (B) connected to 0 (A), 2 (C), and 4 (E)
        {1, 3, 5}, // 2 (C) connected to 1 (B), 3 (D), and 5 (F)
        {2, 4},    // 3 (D) connected to 2 (C) and 4 (E)
        {0, 1, 3}, // 4 (E) connected to 0 (A), 1 (B), and 3 (D)
        {2}        // 5 (F) connected to 2 (C)
    };
    
    // Create and display the graph
    edulcni::create_graph_widget("graph");
    edulcni::set_graph("graph", graph);
    edulcni::step();
    
    visited.assign(graph.size(), false);
    // Run DFS from vertex 0 (equivalent to 'A' in the original example)
    dfs(graph, 0);
    
    // Clear all highlights to show final state
    edulcni::clear_graph_highlights("graph");
    edulcni::step();
    
    std::cout << "Visualization exported to ./graph_viz_output\n";
    std::cout << "Open ./graph_viz_output/index.html in a browser to view\n";
    
    return 0;
} 