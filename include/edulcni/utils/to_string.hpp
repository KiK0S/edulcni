#pragma once

#include <string>
#include <sstream>

namespace edulcni {
namespace internal {

// Helper function to convert any type to string for visualization
template <typename T>
std::string to_string_any(const T& value) {
    if constexpr (std::is_same_v<T, std::string>) {
        return "\"" + value + "\"";
    } else {
        std::ostringstream ss;
        ss << value;
        return ss.str();
    }
}

}
}