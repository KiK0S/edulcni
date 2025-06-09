#include "edulcni/widgets/array.hpp"

namespace edulcni {

void array_highlight(const std::string& id, int index) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::ArrayWidgetBase>(id);
    if (widget) {
        widget->highlight(index);
    }
}

void array_clear_highlights(const std::string& id) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::ArrayWidgetBase>(id);
    if (widget) {
        widget->clear_highlights();
    }
}

// Get widget dimensions
std::pair<double, double> array_dimensions(const std::string& id) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::ArrayWidgetBase>(id);
    if (widget) {
        return {widget->width(), widget->height()};
    }
    return {0, 0};
}

void string_widget(const std::string& id, const std::string& s) {
    array_widget(id, s.begin(), s.end());
}

} // namespace edulcni 