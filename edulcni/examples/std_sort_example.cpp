/**
 * Example using std::sort with a custom comparator and edulcni visualization
 */
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "edulcni.hpp"

// Custom comparator with visualization
class StringComparator {
private:
    std::string compare_id_;
    
public:
    StringComparator(const std::string& compare_id) : compare_id_(compare_id) {}
    
    bool operator()(const std::string& a, const std::string& b) {
        edulcni::string_compare_widget(compare_id_, a, b);
        
        // Character-by-character comparison
        size_t minLength = std::min(a.length(), b.length());
        
        for (size_t i = 0; i < minLength; ++i) {
            edulcni::string_compare_highlight(compare_id_, i, i);
            edulcni::breakpoint();
            
            if (a[i] != b[i]) {
                bool result = a[i] < b[i];
                std::string resultStr = result ? 
                    "'" + std::string(1, a[i]) + "' < '" + std::string(1, b[i]) + "' (return true)" :
                    "'" + std::string(1, a[i]) + "' > '" + std::string(1, b[i]) + "' (return false)";
                
                edulcni::string_compare_highlight(compare_id_, i, i, resultStr);
                edulcni::breakpoint();
                
                return result;
            }
        }
        
        // If all characters match, compare lengths
        if (a.length() != b.length()) {
            bool result = a.length() < b.length();
            std::string resultStr = result ? 
                "First string is shorter (return true)" :
                "Second string is shorter (return false)";
            
            edulcni::string_compare_highlight(compare_id_, -1, -1, resultStr);
            edulcni::breakpoint();
            
            return result;
        }
        
        // Strings are equal
        edulcni::string_compare_highlight(compare_id_, -1, -1, "Strings are equal (return false)");
        edulcni::breakpoint();
        
        return false;
    }
};

// Visualize the sorting process
void visualize_std_sort(std::vector<std::string>& data) {
    edulcni::freeze();
    
    // Display initial array
    edulcni::array_widget("strings", data.begin(), data.end());
    edulcni::breakpoint();
    
    // Sort using custom comparator
    StringComparator comparator("comparison");
    
    // For visualization purposes, we need to intercept the std::sort
    // We'll use our own simple sort algorithm but with the same comparator
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < i; ++j) {
            // Highlight the elements being compared
            edulcni::array_clear_highlights("strings");
            edulcni::array_highlight("strings", i);
            edulcni::array_highlight("strings", j);
            edulcni::breakpoint();
            
            if (comparator(data[i], data[j])) {
                // Element i should go before element j
                std::string temp = data[i];
                for (size_t k = i; k > j; --k) {
                    data[k] = data[k-1];
                }
                data[j] = temp;
                
                // Show the updated array
                edulcni::array_widget("strings", data.begin(), data.end());
                edulcni::array_highlight("strings", j);
                edulcni::breakpoint();
                
                break;
            }
        }
    }
    
    // Display final sorted array
    edulcni::array_clear_highlights("strings");
    edulcni::array_widget("strings", data.begin(), data.end());
    edulcni::string_compare_clear("comparison");
    edulcni::unfreeze();
}

int main() {
    // Initialize edulcni
    edulcni::initialize("./stdsort_viz_output");
    
    // Create sample data
    std::vector<std::string> data = {"banana", "apple", "pear", "orange", "grape", "kiwi", "melon"};
    
    // Sort and visualize
    visualize_std_sort(data);
    
    // Export visualization
    edulcni::export_visualization("./stdsort_viz_output");
    
    std::cout << "Visualization exported to ./stdsort_viz_output\n";
    std::cout << "Open ./stdsort_viz_output/index.html in a browser to view\n";
    
    return 0;
} 