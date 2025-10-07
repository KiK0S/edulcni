/**
 * Comprehensive example demonstrating the geometry widget features
 * Shows points, lines, segments, circles, polygons, halfplanes, vectors, labels,
 * highlighting, zoom-pan, and auto-scaling
 */
#include "edulcni.hpp"
#include <vector>
#include <cmath>

// Define simple point and vector structures
struct Point {
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
};

struct Vector {
    double x, y;
    Vector(double x, double y) : x(x), y(y) {}
};

int main() {
    // Initialize edulcni
    edulcni::initialize("geometry_output");
    edulcni::step();
    
    // Create a geometry widget with grid
    auto id = edulcni::create_geometry_widget("geo1", 800, 600, 20.0);
    edulcni::step();
    
    // Enable zoom-pan functionality
    edulcni::enable_zoom_pan(id, true, 0.1, 10.0);
    edulcni::step();
    
    // Set up coordinate system (flip Y-axis for mathematical convention)
    edulcni::set_coordinate_system(id, true, 400, 300, 1.0, -1.0);
    edulcni::step();
    
    // Draw some points with labels
    Point p1(100, 100);
    Point p2(200, 150);
    Point p3(300, 200);
    Point p4(150, 250);
    
    edulcni::draw_point(id, p1, "red", 5.0, "A");
    edulcni::step();
    
    edulcni::draw_point(id, p2, "blue", 5.0, "B");
    edulcni::step();
    
    edulcni::draw_point(id, p3, "green", 5.0, "C");
    edulcni::step();
    
    edulcni::draw_point(id, p4, "purple", 5.0, "D");
    edulcni::step();
    
    // Draw line segments connecting the points
    edulcni::draw_segment(id, p1, p2, "black", 2.0, "solid");
    edulcni::step();
    
    edulcni::draw_segment(id, p2, p3, "black", 2.0, "solid");
    edulcni::step();
    
    edulcni::draw_segment(id, p3, p4, "black", 2.0, "solid");
    edulcni::step();
    
    edulcni::draw_segment(id, p4, p1, "black", 2.0, "solid");
    edulcni::step();
    
    // Draw a circle
    Point center(250, 250);
    edulcni::draw_circle(id, center, 50.0, "rgba(255, 0, 0, 0.3)", "red", 2.0, true);
    edulcni::step();
    
    // Draw a polygon (triangle)
    std::vector<Point> triangle = {{400, 100}, {450, 200}, {350, 200}};
    edulcni::draw_polygon(id, triangle, "rgba(0, 255, 0, 0.3)", "green", 2.0, true);
    edulcni::step();
    
    // Draw a vector
    Vector dir(50, 30);
    edulcni::draw_vector(id, p1, dir, "purple", 3.0, 10.0, "v1");
    edulcni::step();
    
    // Draw another vector
    Vector dir2(-30, 40);
    edulcni::draw_vector(id, p2, dir2, "orange", 3.0, 10.0, "v2");
    edulcni::step();
    
    // Draw a half-plane (left side of a line)
    Vector line_dir(1, 1);  // Direction vector
    edulcni::draw_halfplane(id, Point(0, 0), line_dir, "rgba(0, 0, 255, 0.2)", "blue");
    edulcni::step();
    
    // Draw another half-plane
    Vector line_dir2(1, -1);
    edulcni::draw_halfplane(id, Point(500, 100), line_dir2, "rgba(255, 165, 0, 0.2)", "orange");
    edulcni::step();
    
    // Highlight a point
    edulcni::highlight_point(id, p2, true, "yellow");
    edulcni::step();
    
    // Draw some additional points in a pattern
    std::vector<Point> pattern_points;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            pattern_points.emplace_back(50 + i * 30, 350 + j * 30);
        }
    }
    
    std::vector<std::string> labels;
    for (size_t i = 0; i < pattern_points.size(); ++i) {
        labels.push_back(std::to_string(i + 1));
    }
    
    edulcni::draw_points(id, pattern_points, "gray", 3.0, labels);
    edulcni::step();
    
    // Draw a dashed line
    edulcni::draw_segment(id, Point(50, 50), Point(350, 350), "red", 2.0, "dashed");
    edulcni::step();
    
    // Draw a dotted line
    edulcni::draw_segment(id, Point(50, 350), Point(350, 50), "blue", 2.0, "dotted");
    edulcni::step();
    
    // Draw an infinite line
    Vector line_direction(1, 0.5);
    edulcni::draw_line(id, Point(200, 100), line_direction, "green", 1.5, "solid");
    edulcni::step();
    
    // Auto-scale the view to fit all data with 10% padding
    edulcni::auto_scale_view(id, 0.1);
    edulcni::step();
    
    // Register a click handler
    edulcni::register_geometry_click_handler(id, [](double x, double y) {
        std::cout << "Clicked at world coordinates: (" << x << ", " << y << ")" << std::endl;
    });
    edulcni::step();
    
    // Demonstrate clearing highlights
    edulcni::clear_highlights(id);
    edulcni::step();
    
    // Highlight a different point
    edulcni::highlight_point(id, p3, true, "cyan");
    edulcni::step();
    
    // Show grid settings
    edulcni::set_grid(id, true, 25.0, "#d0d0d0");
    edulcni::step();
    
    // Draw a complex polygon (star shape)
    std::vector<Point> star;
    for (int i = 0; i < 10; ++i) {
        double angle = i * M_PI / 5.0;
        double radius = (i % 2 == 0) ? 40.0 : 20.0;
        star.emplace_back(600 + radius * cos(angle), 200 + radius * sin(angle));
    }
    edulcni::draw_polygon(id, star, "rgba(255, 255, 0, 0.4)", "gold", 2.0, true);
    edulcni::step();
    
    // Draw multiple circles in a pattern
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            Point circle_center(500 + i * 60, 400 + j * 60);
            edulcni::draw_circle(id, circle_center, 15.0, "transparent", "brown", 1.0, false);
            edulcni::step();
        }
    }
    
    // Final step to show the complete visualization
    edulcni::step();
    
    std::cout << "Geometry widget example completed!" << std::endl;
    std::cout << "Features demonstrated:" << std::endl;
    std::cout << "- Points with labels" << std::endl;
    std::cout << "- Line segments with different styles" << std::endl;
    std::cout << "- Infinite lines" << std::endl;
    std::cout << "- Circles with fill" << std::endl;
    std::cout << "- Polygons (triangle and star)" << std::endl;
    std::cout << "- Half-planes (left side of direction vectors)" << std::endl;
    std::cout << "- Vectors with arrow heads" << std::endl;
    std::cout << "- Point highlighting" << std::endl;
    std::cout << "- Grid and coordinate system" << std::endl;
    std::cout << "- Zoom-pan functionality" << std::endl;
    std::cout << "- Auto-scaling with padding" << std::endl;
    std::cout << "- Click handling" << std::endl;
    
    return 0;
} 