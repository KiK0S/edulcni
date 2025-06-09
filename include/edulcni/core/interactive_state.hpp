/**
 * Interactive state management for edulcni
 * Adds real-time frame streaming and event handling on top of the base State
 */
#pragma once

#include "edulcni/core/state.hpp"
#include <functional>

namespace edulcni {
namespace internal {

class InteractiveState {
private:
    bool interactive_mode_ = false;
    std::function<void()> render_callback_;
    bool auto_render_ = false;
    
    // Private constructor for singleton
    InteractiveState() = default;
    
public:
    // Enable interactive mode
    void enable_interactive_mode();
    
    // Set custom render callback
    void set_render_callback(std::function<void()> callback);
    
    // Request a frame update (triggers re-render)
    void request_frame_update();
    
    // Stream the latest frame to browser
    void stream_frame_to_browser();
    
    // Auto-render on widget changes
    void set_auto_render(bool enabled) { auto_render_ = enabled; }
    bool is_auto_render() const { return auto_render_; }
    
    // Interactive mode check
    bool is_interactive_mode() const { return interactive_mode_; }
    
    // Enhanced step that handles interactive mode
    void interactive_step();
    
    // Singleton access
    static InteractiveState& instance() {
        static InteractiveState state;
        return state;
    }
};

} // namespace internal

// Global functions for interactive mode
void enable_interactive_mode();
void request_frame_update();
void set_auto_render(bool enabled);
void start_interactive_loop();

} // namespace edulcni 