#include "edulcni/core/widget.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/events.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/em_js.h>

EM_JS(void, js_setup_widget_click_handler, (const char* widget_id), {
    console.log('js_setup_widget_click_handler');
    if (window.edulcniJS && window.edulcniJS.setupWidgetClickHandler) {
        window.edulcniJS.setupWidgetClickHandler(UTF8ToString(widget_id));
    } else {
        console.warn("setupWidgetClickHandler function not found in edulcniJS");
    }
});

EM_JS(void, js_setup_global_canvas_click_handler, (), {
    console.log('js_setup_global_canvas_click_handler');
    if (window.edulcniJS && window.edulcniJS.setupGlobalCanvasClickHandler) {
        window.edulcniJS.setupGlobalCanvasClickHandler();
    } else {
        console.warn("setupGlobalCanvasClickHandler function not found in edulcniJS");
    }
});
#endif

namespace edulcni {

// Unified click handling for all widgets
void handle_widget_click(const std::string& widget_id, double x, double y) {
    auto* widget = internal::State::instance().get_widget(widget_id);
    if (!widget) return;
    
    // Check if click is within widget bounds and handle it
    if (widget->contains_point(x, y)) {
        double relative_x = x - widget->x();
        double relative_y = y - widget->y();
        widget->handle_click(relative_x, relative_y);
    }
}

// Register a unified click handler for any widget
void register_widget_click_handler(const std::string& widget_id) {
    std::string event_id = widget_id + "_click";
    register_event_callback(event_id, [widget_id](const EventData& event) {
        handle_widget_click(widget_id, event.x, event.y);
    });
    
    // Also register for the global canvas click event
    register_event_callback("canvas_click", [widget_id](const EventData& event) {
        handle_widget_click(widget_id, event.x, event.y);
    });
    
    #ifdef __EMSCRIPTEN__
    // Setup global canvas click handler (only once)
    static bool global_handler_setup = false;
    if (!global_handler_setup) {
        js_setup_global_canvas_click_handler();
        global_handler_setup = true;
    }
    #endif
}

} // namespace edulcni 