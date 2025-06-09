#include "edulcni/widgets/matrix.hpp"

namespace edulcni {

void matrix_highlight(const std::string& id, int row, int col) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::MatrixWidgetBase>(id);
    if (widget) {
        widget->highlight(row, col);
    }
}

void matrix_clear_highlights(const std::string& id) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::MatrixWidgetBase>(id);
    if (widget) {
        widget->clear_highlights();
    }
}

// Get widget dimensions
std::pair<double, double> matrix_dimensions(const std::string& id) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::MatrixWidgetBase>(id);
    if (widget) {
        return {widget->width(), widget->height()};
    }
    return {0, 0};
}

} // namespace edulcni 