/**
 * Rendering primitives for edulcni
 */
#pragma once

#include <string>
#include <vector>
#include "edulcni/core/json.hpp"

namespace edulcni {
namespace render {

// Color type
struct Color {
    unsigned char r, g, b, a;
    
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
        : r(r), g(g), b(b), a(a) {}
    
    static Color Red() { return Color(255, 0, 0); }
    static Color Green() { return Color(0, 255, 0); }
    static Color Blue() { return Color(0, 0, 255); }
    static Color Yellow() { return Color(255, 255, 0); }
    static Color Black() { return Color(0, 0, 0); }
    static Color White() { return Color(255, 255, 255); }
    static Color Highlight() { return Color(255, 235, 59); } // Yellow highlight
    
    std::string to_css() const {
        std::ostringstream ss;
        if (a == 255) {
            ss << "rgb(" << (int)r << "," << (int)g << "," << (int)b << ")";
        } else {
            ss << "rgba(" << (int)r << "," << (int)g << "," << (int)b << "," << (a / 255.0) << ")";
        }
        return ss.str();
    }
};

// Base class for all rendering elements
class Element {
public:
    virtual ~Element() = default;
    virtual json::value to_json() const = 0;
};

// Container for multiple elements
class Group : public Element {
private:
    std::vector<std::unique_ptr<Element>> children_;
    double x_ = 0, y_ = 0;
    
public:
    Group() = default;
    Group(double x, double y) : x_(x), y_(y) {}
    
    void add(std::unique_ptr<Element> element) {
        children_.push_back(std::move(element));
    }
    
    json::value to_json() const override {
        json::value result = json::object{};
        result["type"] = "group";
        result["x"] = x_;
        result["y"] = y_;
        
        json::value children = json::array{};
        for (const auto& child : children_) {
            children.as_array().push_back(child->to_json());
        }
        result["children"] = std::move(children);
        
        return result;
    }
};

// Rectangle element
class Rectangle : public Element {
private:
    double x_, y_, width_, height_;
    Color fill_color_;
    Color stroke_color_;
    double stroke_width_ = 1.0;
    
public:
    Rectangle(double x, double y, double width, double height, 
              Color fill = Color(255, 255, 255), 
              Color stroke = Color(0, 0, 0),
              double stroke_width = 1.0)
        : x_(x), y_(y), width_(width), height_(height), 
          fill_color_(fill), stroke_color_(stroke), stroke_width_(stroke_width) {}
    
    json::value to_json() const override {
        json::value result = json::object{};
        result["type"] = "rect";
        result["x"] = x_;
        result["y"] = y_;
        result["width"] = width_;
        result["height"] = height_;
        result["fill"] = fill_color_.to_css();
        result["stroke"] = stroke_color_.to_css();
        result["strokeWidth"] = stroke_width_;
        return result;
    }
};

// Text element
class Text : public Element {
private:
    double x_, y_;
    std::string text_;
    std::string font_ = "14px Arial";
    Color color_ = Color(0, 0, 0);
    std::string align_ = "center";
    
public:
    Text(double x, double y, const std::string& text, 
         const std::string& font = "14px Arial",
         Color color = Color(0, 0, 0),
         const std::string& align = "center")
        : x_(x), y_(y), text_(text), font_(font), color_(color), align_(align) {}
    
    json::value to_json() const override {
        json::value result = json::object{};
        result["type"] = "text";
        result["x"] = x_;
        result["y"] = y_;
        result["text"] = text_;
        result["font"] = font_;
        result["fill"] = color_.to_css();
        result["textAlign"] = align_;
        return result;
    }
};

// Line element
class Line : public Element {
private:
    double x1_, y1_, x2_, y2_;
    Color color_;
    double width_ = 1.0;
    
public:
    Line(double x1, double y1, double x2, double y2,
         Color color = Color(0, 0, 0),
         double width = 1.0)
        : x1_(x1), y1_(y1), x2_(x2), y2_(y2), color_(color), width_(width) {}
    
    json::value to_json() const override {
        json::value result = json::object{};
        result["type"] = "line";
        result["x1"] = x1_;
        result["y1"] = y1_;
        result["x2"] = x2_;
        result["y2"] = y2_;
        result["stroke"] = color_.to_css();
        result["strokeWidth"] = width_;
        return result;
    }
};

// Circle element
class Circle : public Element {
private:
    double x_, y_, radius_;
    Color fill_color_;
    Color stroke_color_;
    double stroke_width_ = 1.0;
    
public:
    Circle(double x, double y, double radius,
           Color fill = Color(255, 255, 255),
           Color stroke = Color(0, 0, 0),
           double stroke_width = 1.0)
        : x_(x), y_(y), radius_(radius), 
          fill_color_(fill), stroke_color_(stroke), stroke_width_(stroke_width) {}
    
    json::value to_json() const override {
        json::value result = json::object{};
        result["type"] = "circle";
        result["x"] = x_;
        result["y"] = y_;
        result["radius"] = radius_;
        result["fill"] = fill_color_.to_css();
        result["stroke"] = stroke_color_.to_css();
        result["strokeWidth"] = stroke_width_;
        return result;
    }
};

} // namespace render
} // namespace edulcni 