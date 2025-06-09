#include "edulcni/widgets/compare_v.hpp"

namespace edulcni {

// Public API for compare_v widget
void compare_v_widget(const std::string& id, 
                     const std::string& top_widget_id, 
                     const std::string& bottom_widget_id) {
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::CompareVWidget>(id);
    widget->set_widgets(top_widget_id, bottom_widget_id);
}

void compare_v_set_spacing(const std::string& id, double spacing) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::CompareVWidget>(id);
    if (widget) {
        widget->set_spacing(spacing);
    }
}

} // namespace edulcni 