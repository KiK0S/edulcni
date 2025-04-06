/**
 * String bubble sort example with edulcni
 */
#include <iostream>
#include <vector>
#include <string>
#include "edulcni.hpp"

// Bubble Sort for strings with visualization
void bubble_sort(std::vector<std::string>& arr) {
    edulcni::freeze();
    edulcni::array_widget("string_array", arr.begin(), arr.end());
    edulcni::breakpoint();
    
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            // Highlight the elements being compared
            edulcni::array_clear_highlights("string_array");
            edulcni::array_highlight("string_array", j);
            edulcni::array_highlight("string_array", j + 1);
            
            // Show string comparison
            edulcni::string_compare_widget("comparison", arr[j], arr[j + 1]);
            
            // Character-by-character comparison visualization
            bool swapNeeded = false;
            size_t minLength = std::min(arr[j].length(), arr[j+1].length());
            
            for (size_t k = 0; k < minLength; ++k) {
                edulcni::string_compare_highlight("comparison", k, k);
                edulcni::breakpoint();
                
                if (arr[j][k] != arr[j+1][k]) {
                    swapNeeded = arr[j][k] > arr[j+1][k];
                    std::string result = arr[j][k] > arr[j+1][k] ? 
                        "'" + std::string(1, arr[j][k]) + "' > '" + std::string(1, arr[j+1][k]) + "' (Swap needed)" :
                        "'" + std::string(1, arr[j][k]) + "' < '" + std::string(1, arr[j+1][k]) + "' (No swap)";
                    edulcni::string_compare_highlight("comparison", k, k, result);
                    edulcni::breakpoint();
                    break;
                }
            }
            
            // If all characters match and we haven't broken out yet, compare lengths
            if (arr[j].length() != arr[j+1].length() && swapNeeded == false) {
                swapNeeded = arr[j].length() > arr[j+1].length();
                std::string result = arr[j].length() > arr[j+1].length() ? 
                    "First string is longer (Swap needed)" :
                    "Second string is longer (No swap)";
                edulcni::string_compare_highlight("comparison", -1, -1, result);
                edulcni::breakpoint();
            }
            
            if (swapNeeded) {
                // Swap elements
                std::swap(arr[j], arr[j + 1]);
                edulcni::array_widget("string_array", arr.begin(), arr.end());
                edulcni::array_highlight("string_array", j);
                edulcni::array_highlight("string_array", j + 1);
                edulcni::breakpoint();
            }
        }
    }
    
    // Show the final sorted array
    edulcni::array_clear_highlights("string_array");
    edulcni::array_widget("string_array", arr.begin(), arr.end());
    edulcni::string_compare_clear("comparison");
    edulcni::unfreeze();
}

int main() {
    // Initialize edulcni
    edulcni::initialize("./string_viz_output");
    
    // Create sample data
    std::vector<std::string> data = {"aba", "baa", "aab", "ab"};
    
    // Sort and visualize
    bubble_sort(data);
    
    // Export visualization
    edulcni::export_visualization("./string_viz_output");
    
    std::cout << "Visualization exported to ./string_viz_output\n";
    std::cout << "Open ./string_viz_output/index.html in a browser to view\n";
    
    return 0;
} 