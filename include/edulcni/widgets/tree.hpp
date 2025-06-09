/**
 * Graph widget for edulcni
 */
#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <set>
#include <cmath>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"
#include "edulcni/widgets/graph.hpp"
#include <functional>

namespace edulcni {
namespace internal {

// Type-erased base graph widget
class TreeWidgetBase : public GraphWidgetBase {
public:
    TreeWidgetBase(std::string id) : GraphWidgetBase(std::move(id)) {}
    
    void dfs(int v, int p, const std::vector<std::vector<int>>& adj_list, std::vector<int>& topsort) {
        for (auto to : adj_list[v]) 
            if (to != p)
                dfs(to, v, adj_list, topsort);
        topsort.push_back(v);
    }

    void set_tree(const std::vector<std::vector<int>>& adj_list, int root) {
        // Clear existing data
        vertices_.clear();
        edges_.clear();
        
        // Create vertices
        int n = adj_list.size();
       
        std::vector<int> queue;
        std::vector<int> depth(n, -1);
        queue.push_back(root);
        depth[root] = 1;
        int head = 0;
        while (head != queue.size()) {
            int v = queue[head++];
            for (auto to : adj_list[v]) {
                if (depth[to] != -1)
                    continue;
                queue.push_back(to);
                depth[to] = depth[v] + 1;
            }
        }
        for (int i = 0; i < n; i++) {
            std::cerr << depth[i] << ' ';
        }
        std::cerr << '\n';
        int rows = *std::max_element(depth.begin(), depth.end());
        std::vector<int> cnt_depth(rows, 0);
        for (auto d : depth)
            cnt_depth[d-1]++;
        std::vector<int> topsort;
        
        dfs(root, -1, adj_list, topsort);
        for (int i = 0; i < n; i++) {
            std::cerr << topsort[i] << ' ';
        }
        std::cerr << '\n';
        std::vector<int> done_depth(rows, 0);
        int max_width = *std::max_element(cnt_depth.begin(), cnt_depth.end());
        vertices_.resize(n, Vertex(-1, 0, 0, ""));
        auto append_vertex = [&](int v) {
            int max_gap = (max_width - 1) * 3 * NODE_RADIUS;
            int y = (depth[v] - 1) * 3 * NODE_RADIUS;
            int x = max_gap / 2;
            if (cnt_depth[depth[v]-1] > 1) {
                x = max_gap * done_depth[depth[v]-1] / (cnt_depth[depth[v]-1] - 1);
            }
            done_depth[depth[v]-1]++;
            vertices_[v] = Vertex(v, 50 + x, 50 + y, std::to_string(v));
        };


        for (auto v : topsort) {
            append_vertex(v);
        }
        // Create edges (avoiding duplicates for undirected graphs)
        std::set<std::pair<int, int>> edge_set;
        
        for (int i = 0; i < n; i++) {
            for (int neighbor : adj_list[i]) {
                // Ensure we don't add the same edge twice for undirected graphs
                int min_v = std::min(i, neighbor);
                int max_v = std::max(i, neighbor);
                
                auto edge_pair = std::make_pair(min_v, max_v);
                if (edge_set.find(edge_pair) == edge_set.end()) {
                    edges_.emplace_back(i, neighbor);
                    edge_set.insert(edge_pair);
                }
            }
        }
        
        calculate_dimensions();
    }
};

// Graph widget implementation
class TreeWidget : public TreeWidgetBase {
public:
    TreeWidget(std::string id) : TreeWidgetBase(std::move(id)) {}
};

} // namespace internal

// Function to create a graph widget from an adjacency list
internal::TreeWidget* create_tree_widget(const std::string& id) ;

// Set the graph data from an adjacency list
void set_tree(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list, int root = 0);
} // namespace edulcni 