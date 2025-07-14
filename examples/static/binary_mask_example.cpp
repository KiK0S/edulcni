/**
 * Simple example of edulcni array visualization
 */
#include <iostream>
#include <vector>
#include "edulcni.hpp"


int main() {
    // Initialize edulcni
    edulcni::initialize("./binary_mask_viz_output");
    
    int x = 42;
    edulcni::binary_mask_widget("X", x);
    edulcni::step();

    std::cout << "Visualization exported to ./binary_mask_viz_output\n";
    std::cout << "Open ./binary_mask_viz_output/index.html in a browser to view\n";
    
    return 0;
} 