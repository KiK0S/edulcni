#include "edulcni/widgets/button.hpp"
#include "edulcni/core/events.hpp"
#include <string>
#include <functional>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/em_js.h>

EM_JS(void, js_setup_button_handler, (const char* button_id, const char* event_id), {
    var buttonElement = document.getElementById(UTF8ToString(button_id));
    if (buttonElement) {
        buttonElement.onclick = function() {
            if (window.edulcniJS && window.edulcniJS.invokeCallback) {
                window.edulcniJS.invokeCallback(UTF8ToString(event_id));
            }
        };
    }
});
#endif

namespace edulcni {

// Legacy function for registering button click handlers
void register_button_click_handler(const std::string& id, 
                                 std::function<void()> callback) {
    // Use the unified event system
    std::string event_id = "button_" + id;
    register_callback(event_id, callback);
    
    #ifdef __EMSCRIPTEN__
    // Call the JavaScript function to set up the button handler
    js_setup_button_handler(id.c_str(), event_id.c_str());
    #endif
}

// New button widget functions
void button_widget(const std::string& id, const std::string& text, 
                  std::function<void()> click_handler) {
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::ButtonWidget>(id);
    widget->set_text(text);
    if (click_handler) {
        widget->set_click_handler(click_handler);
        
        // Use the unified widget click handling system
        register_widget_click_handler(id);
    }
}

void button_click(const std::string& id) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::ButtonWidget>(id);
    if (widget) {
        widget->handle_click();
    }
}

} // namespace edulcni
