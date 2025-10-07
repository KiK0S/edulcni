#pragma once

#define DEFER(callback) auto x = edulcni::defer_helper([&]() {
#define END_DEFER }());