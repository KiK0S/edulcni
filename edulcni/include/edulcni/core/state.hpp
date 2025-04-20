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
    bool interactive_ = false;  // New flag for interactive mode
    
    // Private constructor for singleton
    State() = default;
    
public:
    // Singleton access
    static State& instance() {
        static State state;
        return state;
    }
    
    void initialize(const std::string& output_dir, bool interactive = false) {
        if (initialized_) {
            return;
        }
        
        output_dir_ = output_dir;
        frames_.clear();
        widgets_.clear();
        frozen_ = false;  // Start in auto-breakpoint mode
        interactive_ = interactive;  // Set interactive mode
        initialized_ = true;
    }
    
    bool is_interactive() const {
        return interactive_;
    }
    
    void finalize() {
        // Create a final frame if needed
        if (!frames_.empty() && !interactive_) {
            breakpoint();
        }
        
        initialized_ = false;
    }
    
    void breakpoint() {
        if (!initialized_) {
            throw std::runtime_error("edulcni not initialized. Call edulcni::initialize() first.");
        }
        
        // In interactive mode, breakpoint behavior may differ
        if (interactive_) {
            // In interactive mode, breakpoints might pause execution or just log
            return;
        }
        
        // Create a new frame with current widget states and renders
        Frame frame;
        for (const auto& [id, widget] : widgets_) {
            frame.add_widget_state(id, widget->to_json(), widget->render());
        }
        
        frames_.push_back(std::move(frame));
    }
    
    void freeze() {
        frozen_ = true;
    }
    
    void unfreeze() {
        frozen_ = false;
        // We don't automatically create a breakpoint when unfreezing
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

// Public API implementation - update initialize to include interactive parameter
inline void initialize(const std::string& output_dir, bool interactive) {
    internal::State::instance().initialize(output_dir, interactive);
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