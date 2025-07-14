/**
 * Rendering primitives for edulcni
 */
#pragma once

#include <string>
#include <vector>
#include <sstream>

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
    virtual std::string to_canvas_js() const = 0;
    
    // Move the element to the right by the specified amount
    virtual void to_right(double offset) = 0;
    
    // Move the element down by the specified amount
    virtual void to_bottom(double offset) = 0;
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
    
    std::string to_canvas_js() const override {
        std::ostringstream js;
        js << "ctx.save();\n";
        js << "ctx.translate(" << x_ << ", " << y_ << ");\n";
        
        for (const auto& child : children_) {
            js << child->to_canvas_js();
        }
        
        js << "ctx.restore();\n";
        return js.str();
    }
    
    // Move the group and all its children to the right
    void to_right(double offset) override {
        x_ += offset;
    }
    
    // Move the group and all its children down
    void to_bottom(double offset) override {
        y_ += offset;
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
    
    std::string to_canvas_js() const override {
        std::ostringstream js;
        js << "ctx.fillStyle = '" << fill_color_.to_css() << "';\n";
        js << "ctx.fillRect(" << x_ << ", " << y_ << ", " << width_ << ", " << height_ << ");\n";
        if (stroke_width_ > 0) {
            js << "ctx.strokeStyle = '" << stroke_color_.to_css() << "';\n";
            js << "ctx.lineWidth = " << stroke_width_ << ";\n";
            js << "ctx.strokeRect(" << x_ << ", " << y_ << ", " << width_ << ", " << height_ << ");\n";
        }
        return js.str();
    }
    
    // Move the rectangle to the right
    void to_right(double offset) override {
        x_ += offset;
    }
    
    // Move the rectangle down
    void to_bottom(double offset) override {
        y_ += offset;
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
    
    std::string to_canvas_js() const override {
        std::ostringstream js;
        js << "ctx.font = '" << font_ << "';\n";
        js << "ctx.fillStyle = '" << color_.to_css() << "';\n";
        js << "ctx.textAlign = '" << align_ << "';\n";
        js << "ctx.fillText('" << text_ << "', " << x_ << ", " << y_ << ");\n";
        return js.str();
    }
    
    // Move the text to the right
    void to_right(double offset) override {
        x_ += offset;
    }
    
    // Move the text down
    void to_bottom(double offset) override {
        y_ += offset;
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
    
    std::string to_canvas_js() const override {
        std::ostringstream js;
        js << "ctx.strokeStyle = '" << color_.to_css() << "';\n";
        js << "ctx.lineWidth = " << width_ << ";\n";
        js << "ctx.beginPath();\n";
        js << "ctx.moveTo(" << x1_ << ", " << y1_ << ");\n";
        js << "ctx.lineTo(" << x2_ << ", " << y2_ << ");\n";
        js << "ctx.stroke();\n";
        return js.str();
    }
    
    // Move the line to the right
    void to_right(double offset) override {
        x1_ += offset;
        x2_ += offset;
    }
    
    // Move the line down
    void to_bottom(double offset) override {
        y1_ += offset;
        y2_ += offset;
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
    
    std::string to_canvas_js() const override {
        std::ostringstream js;
        js << "ctx.beginPath();\n";
        js << "ctx.arc(" << x_ << ", " << y_ << ", " << radius_ << ", 0, 2 * Math.PI);\n";
        js << "ctx.fillStyle = '" << fill_color_.to_css() << "';\n";
        js << "ctx.fill();\n";
        if (stroke_width_ > 0) {
            js << "ctx.strokeStyle = '" << stroke_color_.to_css() << "';\n";
            js << "ctx.lineWidth = " << stroke_width_ << ";\n";
            js << "ctx.stroke();\n";
        }
        return js.str();
    }
    
    // Move the circle to the right
    void to_right(double offset) override {
        x_ += offset;
    }
    
    // Move the circle down
    void to_bottom(double offset) override {
        y_ += offset;
    }
};

// Transform element for zoom-pan and other transformations
class Transform : public Element {
private:
    double a_, b_, c_, d_, e_, f_;
    
public:
    Transform(double a, double b, double c, double d, double e, double f)
        : a_(a), b_(b), c_(c), d_(d), e_(e), f_(f) {}
    
    std::string to_canvas_js() const override {
        std::ostringstream js;
        js << "ctx.save();\n";
        js << "ctx.transform(" << a_ << ", " << b_ << ", " << c_ << ", " 
           << d_ << ", " << e_ << ", " << f_ << ");\n";
        return js.str();
    }
    
    void to_right(double offset) override {
        e_ += offset;
    }
    
    void to_bottom(double offset) override {
        f_ += offset;
    }
};

} // namespace render
} // namespace edulcni 