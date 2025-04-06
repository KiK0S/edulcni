/**
 * Example showing edulcni working with multiple data types
 */
#include <iostream>
#include <vector>
#include <string>
#include "edulcni.hpp"

int main() {
    // Initialize edulcni
    edulcni::initialize("./mixed_types_output");
    
    // Integer array
    std::vector<int> int_data = {64, 34, 25, 12, 22, 11, 90};
    edulcni::array_widget("integers", int_data.begin(), int_data.end());
    
    // String array
    std::vector<std::string> string_data = {"banana", "apple", "pear", "orange", "grape"};
    edulcni::array_widget("strings", string_data.begin(), string_data.end());
    
    // Double array
    std::vector<double> double_data = {3.14, 2.71, 1.618, 1.414, 2.236};
    edulcni::array_widget("doubles", double_data.begin(), double_data.end());
    
    // Highlight some elements
    edulcni::array_highlight("integers", 2);
    edulcni::array_highlight("strings", 1);
    edulcni::array_highlight("doubles", 0);
    
    // Export visualization
    edulcni::export_visualization("./mixed_types_output");
    
    std::cout << "Visualization exported to ./mixed_types_output\n";
    std::cout << "Open ./mixed_types_output/index.html in a browser to view\n";
    
    return 0;
} 