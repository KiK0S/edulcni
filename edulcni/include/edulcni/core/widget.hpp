/**
 * Widget base class for edulcni
 */
#pragma once

#include <string>
#include <memory>
#include "edulcni/core/json.hpp"

namespace edulcni {
namespace internal {

class Widget {
protected:
    std::string id_;
    
public:
    Widget(std::string id) : id_(std::move(id)) {}
    virtual ~Widget() = default;
    
    const std::string& id() const { return id_; }
    
    virtual json::value to_json() const = 0;
    virtual void update_from_json(const json::value& data) = 0;
    virtual std::string widget_type() const = 0;
};

} // namespace internal
} // namespace edulcni 