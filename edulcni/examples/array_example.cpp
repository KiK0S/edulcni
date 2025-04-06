/**
 * Simple example of edulcni array visualization
 */
#include <iostream>
#include <vector>
#include "edulcni.hpp"

// Bubble Sort implementation with visualization
void bubble_sort(std::vector<int>& arr) {
    edulcni::freeze();
    edulcni::array_widget("array", arr.begin(), arr.end());
    edulcni::breakpoint();
    
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            // Highlight the elements being compared
            edulcni::array_clear_highlights("array");
            edulcni::array_highlight("array", j);
            edulcni::array_highlight("array", j + 1);
            
            edulcni::breakpoint();
            if (arr[j] > arr[j + 1]) {
                // Swap elements
                std::swap(arr[j], arr[j + 1]);
                edulcni::array_widget("array", arr.begin(), arr.end());
                edulcni::array_highlight("array", j);
                edulcni::array_highlight("array", j + 1);
                edulcni::breakpoint();
            }
        }
    }
    
    // Show the final sorted array
    edulcni::array_clear_highlights("array");
    edulcni::array_widget("array", arr.begin(), arr.end());
    edulcni::unfreeze();
}

int main() {
    // Initialize edulcni
    edulcni::initialize("./viz_output");
    
    // Create sample data
    std::vector<int> data = {64, 34, 25, 12, 22, 11, 90};
    
    // Sort and visualize
    bubble_sort(data);
    
    // Export visualization
    edulcni::export_visualization();
    
    std::cout << "Visualization exported to ./viz_output\n";
    std::cout << "Open ./viz_output/index.html in a browser to view\n";
    
    return 0;
} 