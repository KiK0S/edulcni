/**
 * edulcni - Competitive Programming Visualization Library
 */
#pragma once

#include "edulcni/core/state.hpp"
#include "edulcni/core/interactive_state.hpp"
#include "edulcni/core/events.hpp"
#include "edulcni/core/layout.hpp"
#include "edulcni/widgets.hpp"
#include <stdexcept>
#include "build_info.hpp"

namespace edulcni {
    // Core functions
    inline void initialize(const std::string& output_dir) {
        std::cout << "Initializing edulcni with output directory: " << output_dir << std::endl;
        internal::State::instance().initialize(output_dir);
    }
    inline void step() {
        std::cout << "Stepping edulcni" << std::endl;
        
        // Check if interactive mode is enabled
        auto& interactive_state = internal::InteractiveState::instance();
        if (interactive_state.is_interactive_mode()) {
            interactive_state.interactive_step();
        } else {
            internal::State::instance().step();
        }
    }
    inline void alert(const std::string& message) {
        std::cout << "Alerting edulcni with message: " << message << std::endl;
        internal::State::instance().alert(message);
    }
    inline void delay(int delay_ms) {
        std::cout << "Delaying edulcni for " << delay_ms << "ms" << std::endl;
        internal::State::instance().delay(delay_ms);
    }
    inline void console(const std::string& message, const std::string& type) {
        std::cout << "Logging message: " << message << " with type: " << type << std::endl;
        internal::State::instance().console(message, type);
    }


    
    // Interactive mode functions (available in all builds, but only work with emscripten)
    inline void enable_interactive_mode() {
        edulcni::enable_interactive_mode();
    }

    inline void start_interactive_loop() {
        edulcni::start_interactive_loop();
    }
    
    // Event registration functions
    inline void register_click_handler(const std::string& widget_id, ClickCallback callback) {
        edulcni::register_click_handler(widget_id, callback);
    }
    
    inline void register_key_handler(KeyCallback callback) {
        edulcni::register_key_handler(callback);
    }
} 