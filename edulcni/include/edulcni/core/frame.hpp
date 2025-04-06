/**
 * Frame class for edulcni
 */
#pragma once

#include <string>
#include <map>
#include <memory>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/json.hpp"

namespace edulcni {
namespace internal {

class Frame {
private:
    std::map<std::string, json::value> widget_states_;
    
public:
    Frame() = default;
    
    void add_widget_state(const std::string& id, const json::value& state) {
        widget_states_[id] = state;
    }
    
    const std::map<std::string, json::value>& widget_states() const {
        return widget_states_;
    }
    
    json::value to_json() const {
        json::value result = json::object{};
        json::value widgets = json::object{};
        
        for (const auto& [id, state] : widget_states_) {
            widgets[id] = state;
        }
        
        result["widgets"] = std::move(widgets);
        return result;
    }
};

} // namespace internal
} // namespace edulcni 