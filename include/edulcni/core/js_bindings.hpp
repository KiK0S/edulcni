/**
 * JS bindings for edulcni
 */
#pragma once

#include <string>
#ifdef __EMSCRIPTEN__


#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/em_js.h>

namespace edulcni {
namespace internal {

void setup_js_event_handlers();

extern "C" {

void js_alert(const char* message);
void js_console(const char* message, const char* type);
void js_delay(int ms);

}
}
}

#else
void js_alert(const char* message);
void js_console(const char* message, const char* type);
void js_delay(int ms);
#endif