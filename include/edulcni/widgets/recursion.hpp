#pragma once

#include "edulcni/widgets/array.hpp"
#include "edulcni/widgets/label.hpp"
#include "edulcni/widgets/compare_h.hpp"
#include "edulcni/widgets/binary_mask.hpp"
#include "edulcni/utils/stack.hpp"
#include <map>

namespace edulcni {


DEFINE_WIDGET_TYPE(Recursion)

// Recursion widget stack visualization modes
enum class RecursionStackMode {
    FullTree,
    CurrentStack
};

namespace internal {

class RecursionWidget : public Widget {
    std::unique_ptr<Stack<std::string>> stack_;

    static constexpr double FRAME_WIDTH = 200;
    static constexpr double FRAME_HEIGHT = 60;
    static constexpr double FRAME_PADDING = 10;
    static constexpr double LEVEL_SPACING = 80;
    static constexpr double SIBLING_SPACING = 20;



    void calculate_dimensions() override {
        if (stack_->max_height() == -1) {
            width_ = FRAME_WIDTH;
            height_ = FRAME_HEIGHT;
            return;
        }
        width_ = stack_->max_width() * (FRAME_WIDTH + SIBLING_SPACING) - SIBLING_SPACING;
        height_ = stack_->max_height() * (FRAME_HEIGHT + FRAME_PADDING) - FRAME_PADDING;
    }

    std::unique_ptr<render::Element> render() const override {
        auto group = std::make_unique<render::Group>();
        
        if (stack_->max_height() == -1) {
            // Render empty state
            group->add(std::make_unique<render::Rectangle>(
                0, 0, FRAME_WIDTH, FRAME_HEIGHT,
                render::Color::White(), render::Color::Black(), 1.0
            ));
            group->add(std::make_unique<render::Text>(
                FRAME_WIDTH / 2, FRAME_HEIGHT / 2,
                "Empty Recursion Stack", "14px Arial", render::Color::Gray(), "center"
            ));
            return group;
        }
    
        auto nodes_to_render = stack_->get_snapshot();

        std::map<int, double> last_x;
        for (int i = 0; i < nodes_to_render.size(); i++) {
            auto [depth, info] = nodes_to_render[i];
            double y = depth * (FRAME_HEIGHT + FRAME_PADDING);
            render::Color bgColor = (i == nodes_to_render.size() - 1) ? 
                render::Color::Highlight() : render::Color::White();
            double x = (last_x.count(depth) ? last_x[depth] : 0.0) + FRAME_WIDTH + SIBLING_SPACING;
            group->add(std::make_unique<render::Rectangle>(
                x, y, FRAME_WIDTH, FRAME_HEIGHT,
                bgColor, render::Color::Black(), 1.0
            ));
            group->add(std::make_unique<render::Text>(
                x + FRAME_WIDTH / 2, y + 20,
                info, "14px Arial", render::Color::Black(), "center"
            ));
            if (depth > 0) {
                group->add(std::make_unique<render::Line>(
                    x + FRAME_WIDTH / 2, y,
                    last_x[depth - 1] + FRAME_WIDTH / 2, y - FRAME_PADDING,
                    render::Color::Black(), 2.0
                ));
            }
            last_x[depth] = x;
        }

        return group;
    }
public:

    RecursionWidget(std::string id, RecursionStackMode mode) : Widget(std::move(id)) {
        if (mode == RecursionStackMode::FullTree) {
            stack_ = std::make_unique<PersistentStack<std::string>>();
        } else {
            stack_ = std::make_unique<BasicStack<std::string>>();
        }
    }
    void push(const std::string& info) {
        stack_->push(info);
    }

    void pop() {
        stack_->pop();
    }
    
    void clear() {
        stack_->clear();
    }
};

}

RecursionIDType create_recursion_widget(const std::string& id, RecursionStackMode mode = RecursionStackMode::FullTree);

void recursion_push(const RecursionIDType& id, const std::string& info = "");
void recursion_pop(const RecursionIDType& id);
void recursion_clear(const RecursionIDType& id);

#define RECURSION_STACK_FRAME(label, ...) \
    edulcni::recursion_push(label, ##__VA_ARGS__); \
    edulcni::step(); \
    auto _recursion_stack_defer = [&](){ \
        edulcni::recursion_pop(label); \
        edulcni::step(); \
    }; \
    struct _RecursionStackDefer { \
        decltype(_recursion_stack_defer)& f; \
        ~_RecursionStackDefer() { f(); } \
    } _recursion_stack_defer_guard{_recursion_stack_defer};

}