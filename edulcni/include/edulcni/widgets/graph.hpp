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

// Type-erased base graph widget
class GraphWidgetBase : public Widget {
protected:
    std::vector<Vertex> vertices_;
    std::vector<Edge> edges_;
    
    // Constants for layout
    static constexpr double NODE_RADIUS = 20;
    static constexpr double PADDING = 40;
    
public:
    GraphWidgetBase(std::string id) : Widget(std::move(id)) {}
    
    void set_graph(const std::vector<std::vector<int>>& adj_list) {
        // Clear existing data
        vertices_.clear();
        edges_.clear();
        
        // Create vertices
        int n = adj_list.size();
        
        // Position vertices in a circle layout
        auto generate_positions = [n](int vertex_count) {
            std::vector<std::pair<double, double>> positions;
            double radius = 150.0;
            double center_x = 200.0;
            double center_y = 200.0;
            
            for (int i = 0; i < vertex_count; i++) {
                double angle = 2.0 * M_PI * i / vertex_count;
                double x = center_x + radius * cos(angle);
                double y = center_y + radius * sin(angle);
                positions.emplace_back(x, y);
            }
            return positions;
        };
        
        auto positions = generate_positions(n);
        
        // Create vertices
        for (int i = 0; i < n; i++) {
            vertices_.emplace_back(i, positions[i].first, positions[i].second, std::to_string(i));
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
    
    json::value to_json() const override {
        json::value result = json::object{};
        result["type"] = "graph";
        
        // Add vertices
        json::value nodes = json::array{};
        for (const auto& vertex : vertices_) {
            json::value node = json::object{};
            node["id"] = vertex.id;
            node["x"] = vertex.x;
            node["y"] = vertex.y;
            node["label"] = vertex.label;
            node["highlighted"] = vertex.highlighted;
            nodes.as_array().push_back(std::move(node));
        }
        result["vertices"] = std::move(nodes);
        
        // Add edges
        json::value links = json::array{};
        for (const auto& edge : edges_) {
            json::value link = json::object{};
            link["from"] = edge.from;
            link["to"] = edge.to;
            link["highlighted"] = edge.highlighted;
            links.as_array().push_back(std::move(link));
        }
        result["edges"] = std::move(links);
        
        // Add dimensions
        result["width"] = width_;
        result["height"] = height_;
        
        return result;
    }
    
    void update_from_json(const json::value& data) override {
        // Not implemented for now
    }
    
    std::string widget_type() const override {
        return "graph";
    }
    
    // Generate rendering instructions for the graph
    std::unique_ptr<render::Element> render() const override {
        auto group = std::make_unique<render::Group>();
        
        // Render edges first (so they appear behind vertices)
        for (const auto& edge : edges_) {
            const Vertex* from_vertex = nullptr;
            const Vertex* to_vertex = nullptr;
            
            // Find the vertices for this edge
            for (const auto& v : vertices_) {
                if (v.id == edge.from) from_vertex = &v;
                if (v.id == edge.to) to_vertex = &v;
            }
            
            if (from_vertex && to_vertex) {
                // Draw the line
                render::Color edge_color = edge.highlighted ? 
                    render::Color(255, 0, 0) : // Red for highlighted
                    render::Color(100, 100, 100); // Gray for normal
                
                double stroke_width = edge.highlighted ? 2.0 : 1.0;
                
                group->add(std::make_unique<render::Line>(
                    from_vertex->x, from_vertex->y, to_vertex->x, to_vertex->y, 
                    edge_color, stroke_width
                ));
            }
        }
        
        // Render vertices
        for (const auto& vertex : vertices_) {
            // Node background
            render::Color bg_color = vertex.highlighted ? 
                render::Color(255, 235, 59) : // Yellow highlight
                render::Color(200, 200, 255); // Light blue for normal
            
            group->add(std::make_unique<render::Circle>(
                vertex.x, vertex.y, NODE_RADIUS,
                bg_color, render::Color::Black(), 1.0
            ));
            
            // Node label
            group->add(std::make_unique<render::Text>(
                vertex.x, vertex.y, vertex.label,
                "14px Arial", render::Color::Black(), "center"
            ));
        }
        
        return group;
    }
};

// Graph widget implementation
class GraphWidget : public GraphWidgetBase {
public:
    GraphWidget(std::string id) : GraphWidgetBase(std::move(id)) {}
};

} // namespace internal

// Function to create a graph widget from an adjacency list
internal::GraphWidget* create_graph_widget(const std::string& id) {
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::GraphWidget>(id);
    internal::State::instance().update_widget(id);
    return widget;
}

// Set the graph data from an adjacency list
void set_graph(const std::string& graph_id, const std::vector<std::vector<int>>& adj_list) {
    auto* widget = internal::State::instance()
        .get_widget_base<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->set_graph(adj_list);
        internal::State::instance().update_widget(graph_id);
    }
}

// Highlight operations
void highlight_vertex(const std::string& graph_id, int vertex_id, bool highlight = true) {
    auto* widget = internal::State::instance()
        .get_widget_base<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->highlight_vertex(vertex_id, highlight);
        internal::State::instance().update_widget(graph_id);
    }
}

void highlight_edge(const std::string& graph_id, int from, int to, bool highlight = true) {
    auto* widget = internal::State::instance()
        .get_widget_base<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->highlight_edge(from, to, highlight);
        internal::State::instance().update_widget(graph_id);
    }
}

void clear_graph_highlights(const std::string& graph_id) {
    auto* widget = internal::State::instance()
        .get_widget_base<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        widget->clear_highlights();
        internal::State::instance().update_widget(graph_id);
    }
}

// Get widget dimensions
std::pair<double, double> graph_dimensions(const std::string& graph_id) {
    auto* widget = internal::State::instance()
        .get_widget_base<internal::GraphWidgetBase>(graph_id);
    if (widget) {
        return {widget->width(), widget->height()};
    }
    return {0, 0};
}

} // namespace edulcni 