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
    const std::vector<std::string>& data_;
public:
    StringComparator(const std::string& compare_id, const std::vector<std::string>& data) : compare_id_(compare_id), data_(data) {}
    
    bool operator()(const std::string& a, const std::string& b) {
        // edulcni::string_compare_widget(compare_id_, a, b);
        
        // Character-by-character comparison
        size_t minLength = std::min(a.length(), b.length());
        edulcni::array_widget("strings", data_.begin(), data_.end());
        edulcni::step();
        for (size_t i = 0; i < minLength; ++i) {
            // edulcni::string_compare_highlight(compare_id_, i, i);
            edulcni::step();
            
            if (a[i] != b[i]) {
                bool result = a[i] < b[i];
                std::string resultStr = result ? 
                    "'" + std::string(1, a[i]) + "' < '" + std::string(1, b[i]) + "' (return true)" :
                    "'" + std::string(1, a[i]) + "' > '" + std::string(1, b[i]) + "' (return false)";
                
                // edulcni::string_compare_highlight(compare_id_, i, i, resultStr);
                edulcni::step();
                
                return result;
            }
        }
        
        // If all characters match, compare lengths
        if (a.length() != b.length()) {
            bool result = a.length() < b.length();
            std::string resultStr = result ? 
                "First string is shorter (return true)" :
                "Second string is shorter (return false)";
            
            // edulcni::string_compare_highlight(compare_id_, -1, -1, resultStr);
            edulcni::step();
            
            return result;
        }
        
        // Strings are equal
        // edulcni::string_compare_highlight(compare_id_, -1, -1, "Strings are equal (return false)");
        edulcni::step();
        
        return false;
    }
};

// Visualize the sorting process
void visualize_std_sort(std::vector<std::string>& data) {
    
    // Display initial array
    edulcni::array_widget("strings", data.begin(), data.end());
    edulcni::step();
    
    // Sort using custom comparator
    StringComparator comparator("comparison", data);
    std::sort(data.begin(), data.end(), comparator);
    edulcni::array_widget("strings", data.begin(), data.end());
    edulcni::step();
}

int main() {
    // Initialize edulcni
    edulcni::initialize("./stdsort_viz_output");
    
    // Create sample data
    std::vector<std::string> data = {"banana", "apple", "pear", "orange", "grape", "kiwi", "melon"};
    
    // Sort and visualize
    visualize_std_sort(data);
    
    std::cout << "Visualization exported to ./stdsort_viz_output\n";
    std::cout << "Open ./stdsort_viz_output/index.html in a browser to view\n";
    
    return 0;
} 