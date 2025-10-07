#include "edulcni.hpp"
#include <iostream>
#include <vector>

int main() {
    // Initialize edulcni for static visualization
    edulcni::initialize("./graph_strategies_output");
    
    // Example 1: Circular Layout (Default)
    // Simple undirected graph
    std::vector<std::vector<int>> circular_graph = {
        {1, 2, 3},     // Node 0 connects to 1, 2, 3
        {0, 2, 4},     // Node 1 connects to 0, 2, 4
        {0, 1, 3, 4},  // Node 2 connects to 0, 1, 3, 4
        {0, 2, 5},     // Node 3 connects to 0, 2, 5
        {1, 2, 5},     // Node 4 connects to 1, 2, 5
        {3, 4}         // Node 5 connects to 3, 4
    };
    
    edulcni::create_graph_widget("circular_graph");
    edulcni::set_graph("circular_graph", circular_graph);
    edulcni::step();
    
        // Clear previous widget and show tree layout
    edulcni::delete_widget("circular_graph");
    
    // Example 2: Tree Layout
    // Tree structure
    std::vector<std::vector<int>> tree_graph = {
        {1, 2},        // Root (0) has children 1, 2
        {3, 4},        // Node 1 has children 3, 4
        {5, 6},        // Node 2 has children 5, 6
        {},            // Node 3 is a leaf
        {},            // Node 4 is a leaf
        {},            // Node 5 is a leaf
        {}             // Node 6 is a leaf
    };
    
    edulcni::create_graph_widget("tree_graph");
    edulcni::set_tree("tree_graph", tree_graph, 0);  // Root at node 0
    edulcni::step();
    
    // Clear previous widget and show BFS layered layout
    edulcni::delete_widget("tree_graph");
    
    // Example 3: BFS Layered Layout
    // Graph with multiple layers from BFS
    std::vector<std::vector<int>> bfs_graph = {
        {1, 2},        // Start vertex (0) connects to 1, 2
        {0, 3, 4},     // Layer 1: nodes 1, 2
        {0, 3, 5},     // Layer 1: nodes 1, 2
        {1, 2, 6},     // Layer 2: nodes 3, 4, 5
        {1, 6},        // Layer 2: nodes 3, 4, 5
        {2, 6},        // Layer 2: nodes 3, 4, 5
        {3, 4, 5}      // Layer 3: node 6
    };
    
    edulcni::create_graph_widget("bfs_graph");
    edulcni::set_bfs_layered("bfs_graph", bfs_graph, 0);  // Start from node 0
    edulcni::step();
    
    // Clear previous widget and show bipartite layout
    edulcni::delete_widget("bfs_graph");
    
    // Example 4: Bipartite Layout
    // Bipartite graph (can be colored with 2 colors)
    std::vector<std::vector<int>> bipartite_graph = {
        {4, 5, 6},     // Left partition: nodes 0, 1, 2, 3
        {4, 5, 6},     // Left partition: nodes 0, 1, 2, 3
        {4, 5, 6},     // Left partition: nodes 0, 1, 2, 3
        {4, 5, 6},     // Left partition: nodes 0, 1, 2, 3
        {0, 1, 2, 3},  // Right partition: nodes 4, 5, 6
        {0, 1, 2, 3},  // Right partition: nodes 4, 5, 6
        {0, 1, 2, 3}   // Right partition: nodes 4, 5, 6
    };
    
    edulcni::create_graph_widget("bipartite_graph");
    edulcni::set_bipartite("bipartite_graph", bipartite_graph);
    edulcni::step();
    
    // Clear previous widget and show planar layout
    edulcni::delete_widget("bipartite_graph");
    
    // Example 5: Planar Layout
    // Planar graph (can be drawn without edge crossings)
    std::vector<std::vector<int>> planar_graph = {
        {1, 2, 3},     // Outer face: 0-1-2-3-0
        {0, 2, 4},     // Inner face: 1-2-4-1
        {0, 1, 3, 4},  // Inner face: 2-3-4-2
        {0, 2, 4},     // Inner face: 3-4-1-3
        {1, 2, 3}      // Inner face: 4-1-2-4
    };
    
    edulcni::create_graph_widget("planar_graph");
    edulcni::set_planar("planar_graph", planar_graph);
    edulcni::step();
    
    // Clear previous widget and show complex graph with highlighting
    edulcni::delete_widget("planar_graph");
    
    // Example 6: Complex Graph with Highlighting
    // More complex graph to demonstrate highlighting
    std::vector<std::vector<int>> complex_graph = {
        {1, 2, 3, 4},      // Node 0: central hub
        {0, 5, 6},         // Node 1: branch 1
        {0, 7, 8},         // Node 2: branch 2
        {0, 9, 10},        // Node 3: branch 3
        {0, 11, 12},       // Node 4: branch 4
        {1, 13},           // Node 5: leaf
        {1, 14},           // Node 6: leaf
        {2, 15},           // Node 7: leaf
        {2, 16},           // Node 8: leaf
        {3, 17},           // Node 9: leaf
        {3, 18},           // Node 10: leaf
        {4, 19},           // Node 11: leaf
        {4, 20},           // Node 12: leaf
        {5},               // Node 13: leaf
        {6},               // Node 14: leaf
        {7},               // Node 15: leaf
        {8},               // Node 16: leaf
        {9},               // Node 17: leaf
        {10},              // Node 18: leaf
        {11},              // Node 19: leaf
        {12}               // Node 20: leaf
    };
    
    edulcni::create_graph_widget("complex_graph");
    edulcni::set_graph("complex_graph", complex_graph);
    edulcni::step();
    
    // Demonstrate highlighting capabilities
    edulcni::highlight_vertex("complex_graph", 0, true);  // Highlight central node
    edulcni::highlight_edge("complex_graph", 0, 1, true); // Highlight edge 0-1
    edulcni::highlight_edge("complex_graph", 0, 2, true); // Highlight edge 0-2
    edulcni::step();
    
    std::cout << "Graph rendering strategies visualization exported to ./graph_strategies_output" << std::endl;
    std::cout << "Open ./graph_strategies_output/index.html in a browser to view" << std::endl;
    std::cout << std::endl;
    std::cout << "Strategies demonstrated:" << std::endl;
    std::cout << "1. Circular Layout: Default circular arrangement of nodes" << std::endl;
    std::cout << "2. Tree Layout: Hierarchical tree structure with BFS-based positioning" << std::endl;
    std::cout << "3. BFS Layered Layout: Nodes arranged in layers based on BFS traversal" << std::endl;
    std::cout << "4. Bipartite Layout: Two-column layout for bipartite graphs" << std::endl;
    std::cout << "5. Planar Layout: Layout optimized for planar graphs with face detection" << std::endl;
    std::cout << "6. Complex Graph: Demonstrates highlighting capabilities" << std::endl;
    
    return 0;
}
