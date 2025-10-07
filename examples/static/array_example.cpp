/**
 * Simple example of edulcni array visualization
 */
#include <iostream>
#include <vector>
#include "edulcni.hpp"

// Bubble Sort implementation with visualization
void bubble_sort(std::vector<int>& arr) {
    auto id = edulcni::array_widget("array", arr.begin(), arr.end());
    edulcni::step();
    
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            // Highlight the elements being compared
            edulcni::clear_highlights(id);
            edulcni::highlight_single(id, j);
            edulcni::highlight_single(id, j + 1);
            
            edulcni::step();
            if (arr[j] > arr[j + 1]) {
                // Swap elements
                std::swap(arr[j], arr[j + 1]);
                id = edulcni::array_widget(id, arr.begin(), arr.end());
                edulcni::highlight_single(id, j);
                edulcni::highlight_single(id, j + 1);
                edulcni::step();
            }
        }
    }
    
    // Show the final sorted array
    edulcni::clear_highlights(id);
    id = edulcni::array_widget(id, arr.begin(), arr.end());
}

int main() {
    // Initialize edulcni
    edulcni::initialize("./array_viz_output");
    
    // Create sample data
    std::vector<int> data = {64, 34, 25, 12, 22, 11, 90};
    
    // Sort and visualize
    bubble_sort(data);
    
    std::cout << "Visualization exported to ./array_viz_output\n";
    std::cout << "Open ./array_viz_output/index.html in a browser to view\n";
    
    return 0;
} 