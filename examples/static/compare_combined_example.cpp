/**
 * Example demonstrating both horizontal and vertical array comparison with edulcni
 */
#include <iostream>
#include <vector>
#include <algorithm>
#include "edulcni.hpp"

int main() {
    // Initialize edulcni
    edulcni::initialize("./compare_combined_viz_output");
    
    // Create arrays for comparison
    std::vector<int> original = {5, 2, 8, 1, 9, 3};
    std::vector<int> sorted = original;
    std::sort(sorted.begin(), sorted.end());
    
    // Create a reversed copy for additional comparison
    std::vector<int> reversed = sorted;
    std::reverse(reversed.begin(), reversed.end());
    
    // Display each array as a separate widget
    edulcni::vector_widget("original", original);
    edulcni::vector_widget("sorted", sorted);
    edulcni::vector_widget("reversed", reversed);
    
    // Create a horizontal comparison between original and sorted
    edulcni::compare_h_widget("h_comparison", "original", "sorted");
    
    // Create a vertical comparison between sorted and reversed
    edulcni::compare_v_widget("v_comparison", "sorted", "reversed");
    
    // Show initial state
    edulcni::step();
    
    // Highlight the minimum value in all arrays
    int min_val = *std::min_element(original.begin(), original.end());
    
    // Find indices of min_val in each array
    auto it_orig = std::find(original.begin(), original.end(), min_val);
    auto it_sorted = std::find(sorted.begin(), sorted.end(), min_val);
    auto it_reversed = std::find(reversed.begin(), reversed.end(), min_val);
    
    int idx_orig = std::distance(original.begin(), it_orig);
    int idx_sorted = std::distance(sorted.begin(), it_sorted);
    int idx_reversed = std::distance(reversed.begin(), it_reversed);
    
    
    // Show with highlights
    edulcni::step();
    
    std::cout << "Visualization exported to ./compare_combined_viz_output\n";
    std::cout << "Open ./compare_combined_viz_output/index.html in a browser to view\n";
    
    return 0;
} 