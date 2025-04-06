/**
 * State management for edulcni
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include "edulcni/core/widget.hpp"
#include "edulcni/core/frame.hpp"

namespace edulcni {
namespace internal {

class State {
private:
    std::vector<Frame> frames_;
    std::map<std::string, std::unique_ptr<Widget>> widgets_;
    bool frozen_ = false;  // When true, breakpoints are only created manually
    std::string output_dir_;
    bool initialized_ = false;
    
    // Private constructor for singleton
    State() = default;
    
public:
    // Singleton access
    static State& instance() {
        static State state;
        return state;
    }
    
    void initialize(const std::string& output_dir) {
        if (initialized_) {
            return;
        }
        
        output_dir_ = output_dir;
        frames_.clear();
        widgets_.clear();
        frozen_ = false;  // Start in auto-breakpoint mode
        initialized_ = true;
    }
    
    void finalize() {
        // Create a final frame if needed
        if (!frames_.empty()) {
            breakpoint();
        }
        
        initialized_ = false;
    }
    
    void breakpoint() {
        if (!initialized_) {
            throw std::runtime_error("edulcni not initialized. Call edulcni::initialize() first.");
        }
        
        // Create a new frame with current widget states
        Frame frame;
        for (const auto& [id, widget] : widgets_) {
            frame.add_widget_state(id, widget->to_json());
        }
        
        frames_.push_back(std::move(frame));
    }
    
    void freeze() {
        frozen_ = true;
    }
    
    void unfreeze() {
        frozen_ = false;
        // Unlike before, we don't automatically create a breakpoint when unfreezing
        // This lets users control exactly when breakpoints happen
    }
    
    template<typename WidgetT, typename... Args>
    WidgetT* get_or_create_widget(const std::string& id, Args&&... args) {
        if (!initialized_) {
            throw std::runtime_error("edulcni not initialized. Call edulcni::initialize() first.");
        }
        
        auto it = widgets_.find(id);
        if (it != widgets_.end()) {
            // Widget exists, return it after casting
            WidgetT* casted = dynamic_cast<WidgetT*>(it->second.get());
            if (!casted) {
                // Different widget type with same ID - recreate
                auto widget = std::make_unique<WidgetT>(id, std::forward<Args>(args)...);
                WidgetT* result = widget.get();
                widgets_[id] = std::move(widget);
                return result;
            }
            return casted;
        }
        
        // Create new widget
        auto widget = std::make_unique<WidgetT>(id, std::forward<Args>(args)...);
        WidgetT* result = widget.get();
        widgets_[id] = std::move(widget);
        return result;
    }
    
    // Get widget by base type (for common operations)
    template<typename WidgetBaseT>
    WidgetBaseT* get_widget_base(const std::string& id) {
        if (!initialized_) {
            return nullptr;
        }
        
        auto it = widgets_.find(id);
        if (it != widgets_.end()) {
            return dynamic_cast<WidgetBaseT*>(it->second.get());
        }
        
        return nullptr;
    }
    
    void update_widget(const std::string& id) {
        if (!initialized_) {
            return;
        }
        
        // Only automatically create breakpoints if NOT in frozen mode
        if (!frozen_) {
            breakpoint();
        }
        // When frozen, users must call breakpoint() manually
    }
    
    const std::vector<Frame>& frames() const {
        return frames_;
    }
    
    const std::string& output_dir() const {
        return output_dir_;
    }
};

} // namespace internal

// Public API implementation
inline void initialize(const std::string& output_dir) {
    internal::State::instance().initialize(output_dir);
}

inline void finalize() {
    internal::State::instance().finalize();
}

inline void breakpoint() {
    internal::State::instance().breakpoint();
}

inline void freeze() {
    internal::State::instance().freeze();
}

inline void unfreeze() {
    internal::State::instance().unfreeze();
}

} // namespace edulcni 