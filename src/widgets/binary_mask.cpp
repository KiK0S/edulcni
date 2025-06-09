#include "edulcni/widgets/binary_mask.hpp"

namespace edulcni {

void binary_mask_widget(const std::string& id, int x, int max_bits) {
    std::vector<int> bits;
    for (int bit = 0; bit < max_bits; bit++) {
        bits.push_back((x >> (max_bits - bit - 1)) & 1);
    }

    label_widget(id + std::string("_binary_label"), x);
    vector_widget(id + std::string("_binary_bits"), bits);
    compare_h_widget(id, id + std::string("_binary_label"), id + std::string("_binary_bits"));
}

} // namespace edulcni 