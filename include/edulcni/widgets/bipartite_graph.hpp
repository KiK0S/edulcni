#include "edulcni/widgets/compare_v.hpp"
#include "edulcni/widgets/graph.hpp"

namespace edulcni {
void bipartite_graph(vector<vector<int>> L2R) {
    auto* widget = internal::State::instance()
        .get_or_create_widget<internal::LabelWidget>(id);
    widget->update(edulcni::internal::to_string_any(val));
}
}