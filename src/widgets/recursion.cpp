#include "edulcni/widgets/recursion.hpp"
#include "edulcni/core/state.hpp"
#include "edulcni/core/render.hpp"

namespace edulcni {

// Create recursion widget function
RecursionIDType create_recursion_widget(const std::string& id, RecursionStackMode mode) {
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::RecursionWidget>(id, mode);
    return id;
}

void recursion_push(const RecursionIDType& id, const std::string& info) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::RecursionWidget>(id);
    widget->push(info);
}

void recursion_pop(const RecursionIDType& id) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::RecursionWidget>(id);
    widget->pop();
}

void recursion_clear(const RecursionIDType& id) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::RecursionWidget>(id);
    widget->clear();
}


} // namespace edulcni
