#include "edulcni/core/js_bindings.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/em_js.h>

namespace edulcni {
namespace internal {

EM_JS(void, js_alert, (const char* message), {
    alert(
        UTF8ToString(message)
    );
});

EM_JS(void, js_console, (const char* message, const char* type), {
    window.edulcniJS.console(
        UTF8ToString(message), UTF8ToString(type)
    );
});

void js_delay(int ms) {
    emscripten_sleep(ms);
}

EM_JS(void, js_setup_event_handlers, (), {
    console.log('js_setup_event_handlers');
});

// Call this function during initialization
void setup_js_event_handlers() {
    js_setup_event_handlers();
}

} // namespace internal
} // namespace edulcni

#else
void js_alert(const char* message){}
void js_console(const char* message, const char* type){}
void js_delay(int ms){}
#endif