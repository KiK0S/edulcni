/**
 * Frame class for edulcni
 */
#pragma once

#include <string>
#include <map>
#include <memory>
#include <sstream>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/render.hpp"

namespace edulcni {
namespace internal {

class Frame {
private:
    std::map<std::string, std::unique_ptr<render::Element>> widget_renders_;
    render::Bounds bounds_ = render::Bounds::empty();

public:
    Frame() = default;

    void add_widget_state(const std::string& id, std::unique_ptr<render::Element> render) {
        // widget_states_[id] = state;
        if (render) {
            bounds_.expand(render->bounds());
        }
        widget_renders_[id] = std::move(render);
    }

    std::string to_canvas_js() const {
        std::ostringstream js;
        js << "function(ctx, canvas) {\n";
        // js << "    ctx.clearRect(0, 0, canvas.width, canvas.height);\n";
        
        for (const auto& [id, render] : widget_renders_) {
            js << "    // Widget: " << id << "\n";
            js << "    " << render->to_canvas_js();
        }
        
        js << "}\n";
        return js.str();
    }

    const render::Bounds& bounds() const {
        return bounds_;
    }
};

} // namespace internal
} // namespace edulcni 