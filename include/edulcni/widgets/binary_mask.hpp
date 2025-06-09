/**
 * Array widget for edulcni
 */
#pragma once

#include <vector>
#include <unordered_set>
#include <type_traits>
#include <algorithm>
#include <sstream>
#include <string>
#include "edulcni/widgets/array.hpp"
#include "edulcni/widgets/compare_h.hpp"
#include "edulcni/widgets/label.hpp"

namespace edulcni {

void binary_mask_widget(const std::string& id, int x, int max_bits = 10) ;

} // namespace edulcni 