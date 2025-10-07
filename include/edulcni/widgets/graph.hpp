/**
 * Graph widget for edulcni
 */
#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <iostream>
#include <set>
#include <cmath>
#include <functional>
#include <queue>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"

namespace edulcni {
namespace internal {

// Represents a vertex in the graph
struct Vertex {
    int id;
    double x, y;        // Position
    std::string label;  // Display label
    bool highlighted = false;
    
    Vertex(int id, double x, double y, std::string label) 
        : id(id), x(x), y(y), label(std::move(label)) {}
};

// Represents an edge in the graph
struct Edge {
    int from;
    int to;
    bool highlighted = false;
    
    Edge(int from, int to) : from(from), to(to) {}
};

// Base strategy class for graph rendering
class GraphStrategy {
public:
    virtual ~GraphStrategy() = default;
    
    // Layout vertices based on the strategy
    virtual void layout_vertices(std::vector<Vertex>& vertices, 
                                const std::vector<Edge>& edges,
                                const std::vector<std::vector<int>>& adj_list) = 0;
    
    // Render the graph using the strategy
    virtual std::unique_ptr<render::Element> render(const std::vector<Vertex>& vertices,
                                                   const std::vector<Edge>& edges) const = 0;
};

// Circular layout strategy (default)
class CircularStrategy : public GraphStrategy {
public:
    void layout_vertices(std::vector<Vertex>& vertices, 
                        const std::vector<Edge>& edges,
                        const std::vector<std::vector<int>>& adj_list) override {
        int n = vertices.size();
        if (n == 0) return;
        
        double radius = 150.0;
        double center_x = 200.0;
        double center_y = 200.0;
        
        for (int i = 0; i < n; i++) {
            double angle = 2.0 * M_PI * i / n;
            vertices[i].x = center_x + radius * cos(angle);
            vertices[i].y = center_y + radius * sin(angle);
        }
    }
    
    std::unique_ptr<render::Element> render(const std::vector<Vertex>& vertices,
                                           const std::vector<Edge>& edges) const override {
        auto group = std::make_unique<render::Group>();
        
        // Render edges first (so they appear behind vertices)
        for (const auto& edge : edges) {
            const Vertex* from_vertex = nullptr;
            const Vertex* to_vertex = nullptr;
            
            // Find the vertices for this edge
            for (const auto& v : vertices) {
                if (v.id == edge.from) from_vertex = &v;
                if (v.id == edge.to) to_vertex = &v;
            }
            
            if (from_vertex && to_vertex) {
                render::Color edge_color = edge.highlighted ? 
                    render::Color::Red() : render::Color(100, 100, 100);
                
                double stroke_width = edge.highlighted ? 2.0 : 1.0;
                
                group->add(std::make_unique<render::Line>(
                    from_vertex->x, from_vertex->y, to_vertex->x, to_vertex->y, 
                    edge_color, stroke_width
                ));
            }
        }
        
        // Render vertices
        for (const auto& vertex : vertices) {
            render::Color bg_color = vertex.highlighted ? 
                render::Color::Yellow() : render::Color(200, 200, 255);
            
            group->add(std::make_unique<render::Circle>(
                vertex.x, vertex.y, 20,
                bg_color, render::Color::Black(), 1.0
            ));
            
            group->add(std::make_unique<render::Text>(
                vertex.x, vertex.y, vertex.label,
                "14px Arial", render::Color::Black(), "center"
            ));
        }
        
        return group;
    }
};

// Tree layout strategy
class TreeStrategy : public GraphStrategy {
private:
    int root_;
    
public:
    TreeStrategy(int root = 0) : root_(root) {}
    
    void layout_vertices(std::vector<Vertex>& vertices, 
                        const std::vector<Edge>& edges,
                        const std::vector<std::vector<int>>& adj_list) override {
        int n = vertices.size();
        if (n == 0) return;
        
        // Calculate depths using BFS
        std::vector<int> depth(n, -1);
        std::queue<int> q;
        q.push(root_);
        depth[root_] = 0;
        
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            
            for (int neighbor : adj_list[v]) {
                if (depth[neighbor] == -1) {
                    depth[neighbor] = depth[v] + 1;
                    q.push(neighbor);
                }
            }
        }
        
        // Count nodes at each depth
        int max_depth = *std::max_element(depth.begin(), depth.end());
        std::vector<int> nodes_at_depth(max_depth + 1, 0);
        for (int d : depth) {
            nodes_at_depth[d]++;
        }
        
        // Position nodes
        std::vector<int> nodes_placed_at_depth(max_depth + 1, 0);
        for (int i = 0; i < n; i++) {
            int d = depth[i];
            int pos_in_layer = nodes_placed_at_depth[d]++;
            
            double y = 50 + d * 80;
            double x = 50;
            if (nodes_at_depth[d] > 1) {
                double layer_width = (nodes_at_depth[d] - 1) * 60;
                x = 50 + (layer_width * pos_in_layer) / (nodes_at_depth[d] - 1);
            }
            
            vertices[i].x = x;
            vertices[i].y = y;
        }
    }
    
    std::unique_ptr<render::Element> render(const std::vector<Vertex>& vertices,
                                           const std::vector<Edge>& edges) const override {
        auto group = std::make_unique<render::Group>();
        
        // Render edges first
        for (const auto& edge : edges) {
            const Vertex* from_vertex = nullptr;
            const Vertex* to_vertex = nullptr;
            
            for (const auto& v : vertices) {
                if (v.id == edge.from) from_vertex = &v;
                if (v.id == edge.to) to_vertex = &v;
            }
            
            if (from_vertex && to_vertex) {
                render::Color edge_color = edge.highlighted ? 
                    render::Color::Red() : render::Color(100, 100, 100);
                
                double stroke_width = edge.highlighted ? 2.0 : 1.0;
                
                group->add(std::make_unique<render::Line>(
                    from_vertex->x, from_vertex->y, to_vertex->x, to_vertex->y, 
                    edge_color, stroke_width
                ));
            }
        }
        
        // Render vertices
        for (const auto& vertex : vertices) {
            render::Color bg_color = vertex.highlighted ? 
                render::Color::Yellow() : render::Color(200, 200, 255);
            
            group->add(std::make_unique<render::Circle>(
                vertex.x, vertex.y, 20,
                bg_color, render::Color::Black(), 1.0
            ));
            
            group->add(std::make_unique<render::Text>(
                vertex.x, vertex.y, vertex.label,
                "14px Arial", render::Color::Black(), "center"
            ));
        }
        
        return group;
    }
};

// BFS layered layout strategy
class BFSLayeredStrategy : public GraphStrategy {
private:
    int start_vertex_;
    
public:
    BFSLayeredStrategy(int start_vertex = 0) : start_vertex_(start_vertex) {}
    
    void layout_vertices(std::vector<Vertex>& vertices, 
                        const std::vector<Edge>& edges,
                        const std::vector<std::vector<int>>& adj_list) override {
        int n = vertices.size();
        if (n == 0) return;
        
        // BFS to find layers
        std::vector<int> layer(n, -1);
        std::queue<int> q;
        q.push(start_vertex_);
        layer[start_vertex_] = 0;
        
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            
            for (int neighbor : adj_list[v]) {
                if (layer[neighbor] == -1) {
                    layer[neighbor] = layer[v] + 1;
                    q.push(neighbor);
                }
            }
        }
        
        // Count nodes at each layer
        int max_layer = *std::max_element(layer.begin(), layer.end());
        std::vector<int> nodes_at_layer(max_layer + 1, 0);
        for (int l : layer) {
            if (l >= 0) nodes_at_layer[l]++;
        }
        
        // Position nodes
        std::vector<int> nodes_placed_at_layer(max_layer + 1, 0);
        for (int i = 0; i < n; i++) {
            if (layer[i] == -1) {
                // Unreachable nodes go to the right
                vertices[i].x = 400;
                vertices[i].y = 50 + i * 30;
                continue;
            }
            
            int l = layer[i];
            int pos_in_layer = nodes_placed_at_layer[l]++;
            
            double y = 50 + l * 80;
            double x = 50;
            if (nodes_at_layer[l] > 1) {
                double layer_width = (nodes_at_layer[l] - 1) * 60;
                x = 50 + (layer_width * pos_in_layer) / (nodes_at_layer[l] - 1);
            }
            
            vertices[i].x = x;
            vertices[i].y = y;
        }
    }
    
    std::unique_ptr<render::Element> render(const std::vector<Vertex>& vertices,
                                           const std::vector<Edge>& edges) const override {
        auto group = std::make_unique<render::Group>();
        
        // Render edges first
        for (const auto& edge : edges) {
            const Vertex* from_vertex = nullptr;
            const Vertex* to_vertex = nullptr;
            
            for (const auto& v : vertices) {
                if (v.id == edge.from) from_vertex = &v;
                if (v.id == edge.to) to_vertex = &v;
            }
            
            if (from_vertex && to_vertex) {
                render::Color edge_color = edge.highlighted ? 
                    render::Color::Red() : render::Color(100, 100, 100);
                
                double stroke_width = edge.highlighted ? 2.0 : 1.0;
                
                group->add(std::make_unique<render::Line>(
                    from_vertex->x, from_vertex->y, to_vertex->x, to_vertex->y, 
                    edge_color, stroke_width
                ));
            }
        }
        
        // Render vertices
        for (const auto& vertex : vertices) {
            render::Color bg_color = vertex.highlighted ? 
                render::Color::Yellow() : render::Color(200, 200, 255);
            
            group->add(std::make_unique<render::Circle>(
                vertex.x, vertex.y, 20,
                bg_color, render::Color::Black(), 1.0
            ));
            
            group->add(std::make_unique<render::Text>(
                vertex.x, vertex.y, vertex.label,
                "14px Arial", render::Color::Black(), "center"
            ));
        }
        
        return group;
    }
};

// Bipartite graph layout strategy
class BipartiteStrategy : public GraphStrategy {
public:
    void layout_vertices(std::vector<Vertex>& vertices, 
                        const std::vector<Edge>& edges,
                        const std::vector<std::vector<int>>& adj_list) override {
        int n = vertices.size();
        if (n == 0) return;
        
        // Try to find bipartition using BFS
        std::vector<int> color(n, -1);
        std::queue<int> q;
        
        // Start with vertex 0
        color[0] = 0;
        q.push(0);
        
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            
            for (int neighbor : adj_list[v]) {
                if (color[neighbor] == -1) {
                    color[neighbor] = 1 - color[v];
                    q.push(neighbor);
                } else if (color[neighbor] == color[v]) {
                    // Not bipartite, fall back to circular layout
                    CircularStrategy().layout_vertices(vertices, edges, adj_list);
                    return;
                }
            }
        }
        
        // Count nodes in each partition
        int left_count = 0, right_count = 0;
        for (int c : color) {
            if (c == 0) left_count++;
            else if (c == 1) right_count++;
        }
        
        // Position nodes in two columns
        std::vector<int> left_pos(0), right_pos(0);
        for (int i = 0; i < n; i++) {
            if (color[i] == 0) left_pos.push_back(i);
            else if (color[i] == 1) right_pos.push_back(i);
        }
        
        // Position left partition
        for (size_t i = 0; i < left_pos.size(); i++) {
            int v = left_pos[i];
            vertices[v].x = 100;
            vertices[v].y = 50 + i * 60;
        }
        
        // Position right partition
        for (size_t i = 0; i < right_pos.size(); i++) {
            int v = right_pos[i];
            vertices[v].x = 300;
            vertices[v].y = 50 + i * 60;
        }
    }
    
    std::unique_ptr<render::Element> render(const std::vector<Vertex>& vertices,
                                           const std::vector<Edge>& edges) const override {
        auto group = std::make_unique<render::Group>();
        
        // Render edges first
        for (const auto& edge : edges) {
            const Vertex* from_vertex = nullptr;
            const Vertex* to_vertex = nullptr;
            
            for (const auto& v : vertices) {
                if (v.id == edge.from) from_vertex = &v;
                if (v.id == edge.to) to_vertex = &v;
            }
            
            if (from_vertex && to_vertex) {
                render::Color edge_color = edge.highlighted ? 
                    render::Color::Red() : render::Color(100, 100, 100);
                
                double stroke_width = edge.highlighted ? 2.0 : 1.0;
                
                group->add(std::make_unique<render::Line>(
                    from_vertex->x, from_vertex->y, to_vertex->x, to_vertex->y, 
                    edge_color, stroke_width
                ));
            }
        }
        
        // Render vertices
        for (const auto& vertex : vertices) {
            render::Color bg_color = vertex.highlighted ? 
                render::Color::Yellow() : render::Color(200, 200, 255);
            
            group->add(std::make_unique<render::Circle>(
                vertex.x, vertex.y, 20,
                bg_color, render::Color::Black(), 1.0
            ));
            
            group->add(std::make_unique<render::Text>(
                vertex.x, vertex.y, vertex.label,
                "14px Arial", render::Color::Black(), "center"
            ));
        }
        
        return group;
    }
};

// Planar graph layout strategy
class PlanarStrategy : public GraphStrategy {
public:
    void layout_vertices(std::vector<Vertex>& vertices, 
                        const std::vector<Edge>& edges,
                        const std::vector<std::vector<int>>& adj_list) override {
        // For now, use circular layout as fallback
        // In a full implementation, this would use a proper planar embedding algorithm
        CircularStrategy().layout_vertices(vertices, edges, adj_list);
    }
    
    std::unique_ptr<render::Element> render(const std::vector<Vertex>& vertices,
                                           const std::vector<Edge>& edges) const override {
        auto group = std::make_unique<render::Group>();
        
        // Build adjacency list for face detection
        std::unordered_map<int, std::vector<int>> adj;
        for (const auto& edge : edges) {
            adj[edge.from].push_back(edge.to);
            adj[edge.to].push_back(edge.from);
        }
        
        // Simple face detection (naive approach)
        std::vector<std::vector<int>> faces;
        std::set<std::set<int>> seen_faces;
        
        auto is_new_face = [&](const std::vector<int>& face) {
            std::set<int> s(face.begin(), face.end());
            return seen_faces.insert(s).second;
        };
        
        // Find faces using DFS
        for (const auto& edge : edges) {
            int u = edge.from, v = edge.to;
            std::vector<int> path = {u, v};
            std::set<int> visited = {u, v};
            bool found = false;
            
            std::function<void(int,int)> dfs = [&](int curr, int prev) {
                if (found) return;
                for (int w : adj[curr]) {
                    if (w == prev) continue;
                    if (w == u && path.size() > 2) {
                        if (is_new_face(path)) {
                            faces.push_back(path);
                        }
                        found = true;
                        return;
                    }
                    if (!visited.count(w)) {
                        path.push_back(w);
                        visited.insert(w);
                        dfs(w, curr);
                        path.pop_back();
                        visited.erase(w);
                    }
                }
            };
            dfs(v, u);
        }
        
        // Draw faces as polygons
        for (const auto& face : faces) {
            std::vector<std::pair<double,double>> pts;
            for (int idx : face) {
                for (const auto& v : vertices) {
                    if (v.id == idx) {
                        pts.emplace_back(v.x, v.y);
                        break;
                    }
                }
            }
            
            if (pts.size() >= 3) {
                // Create polygon outline
                for (size_t i = 0; i < pts.size(); i++) {
                    size_t next = (i + 1) % pts.size();
                    group->add(std::make_unique<render::Line>(
                        pts[i].first, pts[i].second, pts[next].first, pts[next].second,
                        render::Color(173, 216, 230, 100), 1.0
                    ));
                }
            }
        }
        
        // Draw edges
        for (const auto& edge : edges) {
            const Vertex* from_vertex = nullptr;
            const Vertex* to_vertex = nullptr;
            
            for (const auto& v : vertices) {
                if (v.id == edge.from) from_vertex = &v;
                if (v.id == edge.to) to_vertex = &v;
            }
            
            if (from_vertex && to_vertex) {
                render::Color edge_color = edge.highlighted ? 
                    render::Color::Red() : render::Color::Black();
                
                double stroke_width = edge.highlighted ? 2.0 : 1.0;
                
                group->add(std::make_unique<render::Line>(
                    from_vertex->x, from_vertex->y, to_vertex->x, to_vertex->y, 
                    edge_color, stroke_width
                ));
            }
        }
        
        // Draw vertices
        for (const auto& vertex : vertices) {
            render::Color bg_color = vertex.highlighted ? 
                render::Color::Yellow() : render::Color::White();
            
            group->add(std::make_unique<render::Circle>(
                vertex.x, vertex.y, 18,
                bg_color, render::Color::Black(), 2.0
            ));
            
            group->add(std::make_unique<render::Text>(
                vertex.x, vertex.y, vertex.label,
                "16px Arial", render::Color::Black(), "center"
            ));
        }
        
        return group;
    }
};

// Type-erased base graph widget
class GraphWidgetBase : public Widget {
protected:
    std::vector<Vertex> vertices_;
    std::vector<Edge> edges_;
    std::unique_ptr<GraphStrategy> strategy_;
    
    // Constants for layout
    static constexpr double NODE_RADIUS = 20;
    static constexpr double PADDING = 40;
    
public:
    GraphWidgetBase(std::string id) : Widget(std::move(id)) {
        // Default to circular strategy
        strategy_ = std::make_unique<CircularStrategy>();
    }
    
    void set_strategy(std::unique_ptr<GraphStrategy> strategy) {
        strategy_ = std::move(strategy);
        if (!vertices_.empty()) {
            strategy_->layout_vertices(vertices_, edges_, get_adjacency_list());
            calculate_dimensions();
        }
    }
    
    void set_graph(const std::vector<std::vector<int>>& adj_list) {
        // Clear existing data
        vertices_.clear();
        edges_.clear();
        
        // Create vertices
        int n = adj_list.size();
        for (int i = 0; i < n; i++) {
            vertices_.emplace_back(i, 0, 0, std::to_string(i));
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
        
        // Layout vertices using current strategy
        strategy_->layout_vertices(vertices_, edges_, adj_list);
        calculate_dimensions();
    }
    
    void set_tree(const std::vector<std::vector<int>>& adj_list, int root = 0) {
        set_strategy(std::make_unique<TreeStrategy>(root));
        set_graph(adj_list);
    }
    
    void set_bfs_layered(const std::vector<std::vector<int>>& adj_list, int start = 0) {
        set_strategy(std::make_unique<BFSLayeredStrategy>(start));
        set_graph(adj_list);
    }
    
    void set_bipartite(const std::vector<std::vector<int>>& adj_list) {
        set_strategy(std::make_unique<BipartiteStrategy>());
        set_graph(adj_list);
    }
    
    void set_planar(const std::vector<std::vector<int>>& adj_list) {
        set_strategy(std::make_unique<PlanarStrategy>());
        set_graph(adj_list);
    }
    
    void highlight_vertex(int id, bool highlight = true) {
        for (auto& vertex : vertices_) {
            if (vertex.id == id) {
                vertex.highlighted = highlight;
                break;
            }
        }
    }
    
    void highlight_edge(int from, int to, bool highlight = true) {
        for (auto& edge : edges_) {
            // Check both directions for undirected graphs
            if ((edge.from == from && edge.to == to) || 
                (edge.from == to && edge.to == from)) {
                edge.highlighted = highlight;
                break;
            }
        }
    }
    
    void clear_highlights() {
        for (auto& vertex : vertices_) {
            vertex.highlighted = false;
        }
        
        for (auto& edge : edges_) {
            edge.highlighted = false;
        }
    }
    
    // Calculate dimensions based on vertex positions
    void calculate_dimensions() override {
        if (vertices_.empty()) {
            width_ = 400;  // Default width
            height_ = 400; // Default height
            return;
        }
        
        double min_x = std::numeric_limits<double>::max();
        double min_y = std::numeric_limits<double>::max();
        double max_x = std::numeric_limits<double>::lowest();
        double max_y = std::numeric_limits<double>::lowest();
        
        for (const auto& vertex : vertices_) {
            min_x = std::min(min_x, vertex.x);
            min_y = std::min(min_y, vertex.y);
            max_x = std::max(max_x, vertex.x);
            max_y = std::max(max_y, vertex.y);
        }
        
        width_ = (max_x - min_x) + 2 * (NODE_RADIUS + PADDING);
        height_ = (max_y - min_y) + 2 * (NODE_RADIUS + PADDING);
    }

    // Generate rendering instructions for the graph
    std::unique_ptr<render::Element> render() const override {
        return strategy_->render(vertices_, edges_);
    }
    
private:
    std::vector<std::vector<int>> get_adjacency_list() const {
        std::vector<std::vector<int>> adj_list(vertices_.size());
        for (const auto& edge : edges_) {
            adj_list[edge.from].push_back(edge.to);
            adj_list[edge.to].push_back(edge.from);
        }
        return adj_list;
    }
};

// Graph widget implementation
class GraphWidget : public GraphWidgetBase {
public:
    GraphWidget(std::string id) : GraphWidgetBase(std::move(id)) {}
};

} // namespace internal

// Function to create a graph widget from an adjacency list
internal::GraphWidget* create_graph_widget(const std::string& id);

// Set the graph data from an adjacency list
void set_graph(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list);

// Set graph with specific strategy
void set_tree(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list, int root = 0);
void set_bfs_layered(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list, int start = 0);
void set_bipartite(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list);
void set_planar(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list);

// Highlight operations
void highlight_vertex(const std::string& graph_id, int vertex_id, bool highlight = true);
void highlight_edge(const std::string& graph_id, int from, int to, bool highlight = true);
void clear_graph_highlights(const std::string& graph_id);

// Get widget dimensions
std::pair<double, double> graph_dimensions(const std::string& graph_id);

} // namespace edulcni 