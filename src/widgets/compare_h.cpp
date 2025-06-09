#include "edulcni/widgets/compare_h.hpp"

namespace edulcni {

// Public API for compare_h widget
void compare_h_widget(const std::string& id, 
                     const std::string& left_widget_id, 
                     const std::string& right_widget_id) {
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::CompareHWidget>(id);
    widget->set_widgets(left_widget_id, right_widget_id);
}

void compare_h_set_spacing(const std::string& id, double spacing) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::CompareHWidget>(id);
    if (widget) {
        widget->set_spacing(spacing);
    }
}

} // namespace edulcni 