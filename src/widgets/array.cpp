#include "edulcni/widgets/array.hpp"

namespace edulcni {

ArrayIDType array_widget(const ArrayIDType& id, std::initializer_list<int> v) {
    return array_widget(id, v.begin(), v.end());
}

ArrayIDType string_widget(const ArrayIDType& id, const std::string& s) {
    return array_widget(id, s.begin(), s.end());
}

ArrayIDType bigint_widget(const ArrayIDType& id, const std::string& s) {
    return array_widget(id, s.begin(), s.end());
}

ArrayIDType bitint_widget(const ArrayIDType& id, int x, int bits = -1) {
    std::vector<int> v;
    for (int i = 0; (1 << i) <= x || i < bits; i++)
        v.push_back((x >> i) & 1);
    return array_widget(id, v.begin(), v.end());
}

void highlight_single(const ArrayIDType& id, int index) {
    clear_highlights(id);
    auto* widget = internal::State::instance().get_widget_typed<internal::ArrayWidgetBase>(id);
    widget->highlight(index);
}

void highlight_subsegment(const ArrayIDType& id, int start, int end) {
    clear_highlights(id);
    auto* widget = internal::State::instance().get_widget_typed<internal::ArrayWidgetBase>(id);
    for (int i = start; i <= end; i++)
        widget->highlight(i);
}
template<typename Iterator>
void highlight_subsequence(const ArrayIDType& id, Iterator begin, Iterator end) {
    clear_highlights(id);
    auto* widget = internal::State::instance().get_widget_typed<internal::ArrayWidgetBase>(id);
    for (auto it = begin; it != end; it++)
        widget->highlight(*it);
}

// Overload for brace-enclosed initializer list
void highlight_subsequence(const ArrayIDType& id, std::initializer_list<int> indices) {
    clear_highlights(id);
    auto* widget = internal::State::instance().get_widget_typed<internal::ArrayWidgetBase>(id);
    for (int idx : indices)
        widget->highlight(idx);
}

void clear_highlights(const ArrayIDType& id) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::ArrayWidgetBase>(id);
    if (widget) {
        widget->clear_highlights();
    }
}

// Get widget dimensions
std::pair<double, double> array_dimensions(const ArrayIDType& id) {
    auto* widget = internal::State::instance()
        .get_widget_typed<internal::ArrayWidgetBase>(id);
    if (widget) {
        return {widget->width(), widget->height()};
    }
    return {0, 0};
}
} // namespace edulcni 