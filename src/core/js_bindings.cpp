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

//     // Make sure edulcniJS exists
//     if (!window.edulcniJS) {
//         window.edulcniJS = {};
//     }
    
//     // Add the invokeCallback function (for simple callbacks)
//     window.edulcniJS.invokeCallback = function(eventId) {
//         // This calls back into C++
//         Module.ccall('invoke_callback', 'void', ['string'], [eventId]);
//     };
    
//     // Add the invokeCallbackWithData function (for event data)
//     window.edulcniJS.invokeCallbackWithData = function(eventId, eventData) {
//         // This calls back into C++ with JSON event data
//         var jsonData = typeof eventData === 'string' ? eventData : JSON.stringify(eventData);
//         Module.ccall('invoke_callback_with_data', 'void', ['string', 'string'], [eventId, jsonData]);
//     };
    
//     // Setup global key handler
//     document.addEventListener('keydown', function(e) {
//         var eventData = {
//             type: 'keypress',
//             key: e.key,
//             target_id: 'global'
//         };
//         window.edulcniJS.invokeCallbackWithData('global_key', eventData);
//     });
    
//     // Add the setupWidgetClickHandler function for general widgets
//     window.edulcniJS.setupWidgetClickHandler = function(widgetId) {
//         var element = document.getElementById(widgetId);
//         if (element) {
//             element.addEventListener('click', function(e) {
//                 var eventData = {
//                     type: 'click',
//                     x: e.offsetX,
//                     y: e.offsetY,
//                     button: e.button,
//                     target_id: widgetId
//                 };
//                 window.edulcniJS.invokeCallbackWithData(widgetId + '_click', eventData);
//             });
//         }
//     };
// });

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