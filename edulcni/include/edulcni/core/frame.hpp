/**
 * Frame class for edulcni
 */
#pragma once

#include <string>
#include <map>
#include <memory>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/json.hpp"
#include "edulcni/core/render.hpp"

namespace edulcni {
namespace internal {

class Frame {
private:
    std::map<std::string, json::value> widget_states_;
    std::map<std::string, std::unique_ptr<render::Element>> widget_renders_;
    
public:
    Frame() = default;
    
    void add_widget_state(const std::string& id, const json::value& state, 
                         std::unique_ptr<render::Element> render) {
        widget_states_[id] = state;
        widget_renders_[id] = std::move(render);
    }
    
    const std::map<std::string, json::value>& widget_states() const {
        return widget_states_;
    }
    
    json::value to_json() const {
        json::value result = json::object{};
        json::value widgets = json::object{};
        json::value renders = json::object{};
        
        for (const auto& [id, state] : widget_states_) {
            widgets[id] = state;
        }
        
        for (const auto& [id, render] : widget_renders_) {
            renders[id] = render->to_json();
        }
        
        result["widgets"] = std::move(widgets);
        result["renders"] = std::move(renders);
        return result;
    }
};

} // namespace internal
} // namespace edulcni 