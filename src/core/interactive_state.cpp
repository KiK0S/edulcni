#include "edulcni/core/interactive_state.hpp"
#include "edulcni/core/js_bindings.hpp"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/em_js.h>

// JavaScript function to stream frame to browser
EM_JS(void, js_stream_frame, (const char* frame_js), {
    var frameJs = UTF8ToString(frame_js);
    
    try {
        // Evaluate the JavaScript function
        var frameFunction = eval("(" + frameJs + ")");
        
        // Call the frame ready handler if available
        if (window.edulcniJS && window.edulcniJS.onFrameReady) {
            window.edulcniJS.onFrameReady(frameFunction);
        } else if (window.appendFrame && typeof window.appendFrame === 'function') {
            window.appendFrame(frameFunction);
        } else {
            // Fallback: apply frame directly to canvas if available
            var canvas = document.getElementById('main-canvas');
            if (canvas && canvas.getContext) {
                var ctx = canvas.getContext('2d');
                frameFunction(ctx, canvas);
            }
        }
    } catch (error) {
        console.error('Error streaming frame:', error);
    }
});

// Emscripten main loop callback
void emscripten_main_loop_callback() {
    // This function is called by emscripten's main loop
    // We can use it to handle continuous rendering if needed
}

#endif

namespace edulcni {
namespace internal {

void InteractiveState::enable_interactive_mode() {
    if (interactive_mode_) return;
    
    interactive_mode_ = true;
    std::cout << "Interactive mode enabled" << std::endl;
    
    #ifdef __EMSCRIPTEN__
    // Setup JavaScript event handlers
    setup_js_event_handlers();
    #endif
}

void InteractiveState::set_render_callback(std::function<void()> callback) {
    render_callback_ = callback;
}

void InteractiveState::request_frame_update() {
    if (!interactive_mode_) return;
    
    // Call custom render callback if set
    if (render_callback_) {
        render_callback_();
    }
    
    // Create and stream the frame
    interactive_step();
}

void InteractiveState::stream_frame_to_browser() {
    if (!interactive_mode_) return;
    
    #ifdef __EMSCRIPTEN__
    // Get the latest frame from the main state and convert it to JavaScript
    const auto& frames = State::instance().frames();
    std::cout << "frames.size(): " << frames.size() << std::endl;
    if (frames.empty()) return;
    const Frame& latest_frame = frames.back();
    std::string frame_js = latest_frame.to_canvas_js();
    
    // Stream to browser
    js_stream_frame(frame_js.c_str());
    #endif
}

void InteractiveState::interactive_step() {
    // Call the main state step to create frame
    State::instance().step();
    
    // If in interactive mode, stream to browser
    if (interactive_mode_) {
        stream_frame_to_browser();
    }
}

} // namespace internal

// Global functions
void enable_interactive_mode() {
    internal::InteractiveState::instance().enable_interactive_mode();
}

void request_frame_update() {
    internal::InteractiveState::instance().request_frame_update();
}

void set_auto_render(bool enabled) {
    internal::InteractiveState::instance().set_auto_render(enabled);
}

void start_interactive_loop() {
    #ifdef __EMSCRIPTEN__
    std::cout << "Starting interactive event loop" << std::endl;
    
    // Set the main loop callback
    emscripten_set_main_loop(emscripten_main_loop_callback, 0, 1);
    #else
    std::cout << "Interactive loop not supported in non-emscripten builds" << std::endl;
    #endif
}

} // namespace edulcni 