/**
 * Simple JSON implementation for edulcni
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <sstream>
#include <iomanip>

namespace edulcni {
namespace json {

// Forward declaration
class value;

using null_t = std::monostate;
using array = std::vector<value>;
using object = std::map<std::string, value>;

class value {
private:
    std::variant<null_t, bool, int, double, std::string, array, object> data_;

public:
    value() : data_(null_t{}) {}
    value(std::nullptr_t) : data_(null_t{}) {}
    value(bool b) : data_(b) {}
    value(int i) : data_(i) {}
    value(double d) : data_(d) {}
    value(const char* s) : data_(std::string(s)) {}
    value(std::string s) : data_(std::move(s)) {}
    value(array a) : data_(std::move(a)) {}
    value(object o) : data_(std::move(o)) {}

    // Accessors
    bool is_null() const { return std::holds_alternative<null_t>(data_); }
    bool is_bool() const { return std::holds_alternative<bool>(data_); }
    bool is_int() const { return std::holds_alternative<int>(data_); }
    bool is_double() const { return std::holds_alternative<double>(data_); }
    bool is_string() const { return std::holds_alternative<std::string>(data_); }
    bool is_array() const { return std::holds_alternative<array>(data_); }
    bool is_object() const { return std::holds_alternative<object>(data_); }

    bool& as_bool() { return std::get<bool>(data_); }
    int& as_int() { return std::get<int>(data_); }
    double& as_double() { return std::get<double>(data_); }
    std::string& as_string() { return std::get<std::string>(data_); }
    array& as_array() { return std::get<array>(data_); }
    object& as_object() { return std::get<object>(data_); }

    const bool& as_bool() const { return std::get<bool>(data_); }
    const int& as_int() const { return std::get<int>(data_); }
    const double& as_double() const { return std::get<double>(data_); }
    const std::string& as_string() const { return std::get<std::string>(data_); }
    const array& as_array() const { return std::get<array>(data_); }
    const object& as_object() const { return std::get<object>(data_); }

    // Array/object accessors
    value& operator[](size_t index) {
        if (!is_array()) {
            data_ = array{};
        }
        auto& arr = as_array();
        if (index >= arr.size()) {
            arr.resize(index + 1);
        }
        return arr[index];
    }

    value& operator[](const std::string& key) {
        if (!is_object()) {
            data_ = object{};
        }
        return as_object()[key];
    }

    // Serialization
    std::string to_string() const {
        std::ostringstream oss;
        serialize(oss);
        return oss.str();
    }

private:
    void serialize(std::ostream& os) const {
        std::visit([&os](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, null_t>) {
                os << "null";
            } else if constexpr (std::is_same_v<T, bool>) {
                os << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int>) {
                os << val;
            } else if constexpr (std::is_same_v<T, double>) {
                os << std::fixed << std::setprecision(6) << val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                os << '"';
                for (char c : val) {
                    switch (c) {
                        case '"':  os << "\\\""; break;
                        case '\\': os << "\\\\"; break;
                        case '\b': os << "\\b";  break;
                        case '\f': os << "\\f";  break;
                        case '\n': os << "\\n";  break;
                        case '\r': os << "\\r";  break;
                        case '\t': os << "\\t";  break;
                        default:
                            if ('\x00' <= c && c <= '\x1f') {
                                os << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                            } else {
                                os << c;
                            }
                    }
                }
                os << '"';
            } else if constexpr (std::is_same_v<T, array>) {
                os << '[';
                bool first = true;
                for (const auto& elem : val) {
                    if (!first) os << ',';
                    first = false;
                    elem.serialize(os);
                }
                os << ']';
            } else if constexpr (std::is_same_v<T, object>) {
                os << '{';
                bool first = true;
                for (const auto& [k, v] : val) {
                    if (!first) os << ',';
                    first = false;
                    os << '"' << k << "\":";
                    v.serialize(os);
                }
                os << '}';
            }
        }, data_);
    }
};

// Add this function if it doesn't exist
inline std::string stringify(const value& val) {
    return val.to_string();
}

} // namespace json
} // namespace edulcni 