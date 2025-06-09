/**
 * Example demonstrating edulcni matrix visualization
 */
#include <iostream>
#include <vector>
#include <string>
#include "edulcni.hpp"

// Function to find the largest element in a matrix with visualization
template<typename T>
std::pair<int, int> find_largest(const std::vector<std::vector<T>>& matrix) {
    edulcni::matrix_widget("matrix", matrix);
    edulcni::step();
    
    std::pair<int, int> max_pos = {0, 0};
    T max_value = matrix[0][0];
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            // Highlight current cell being examined
            edulcni::matrix_clear_highlights("matrix");
            edulcni::matrix_highlight("matrix", i, j);
            edulcni::step();
            
            if (matrix[i][j] > max_value) {
                max_value = matrix[i][j];
                max_pos = {i, j};
                
                // Highlight the new maximum
                edulcni::matrix_clear_highlights("matrix");
                edulcni::matrix_highlight("matrix", i, j);
                edulcni::step();
            }
        }
    }
    
    // Final highlight showing the largest element
    edulcni::matrix_clear_highlights("matrix");
    edulcni::matrix_highlight("matrix", max_pos.first, max_pos.second);
    edulcni::step();
    
    return max_pos;
}

int main() {
    // Initialize edulcni
    edulcni::initialize("./matrix_viz_output");
    
    // Create a sample integer matrix
    std::vector<std::vector<int>> int_matrix = {
        {4, 12, 9, 7},
        {5, 23, 10, 3},
        {8, 15, 45, 6},
        {11, 18, 13, 2}
    };
    
    // Find and visualize the largest element
    std::pair<int, int> max_pos = find_largest(int_matrix);
    
    std::cout << "Largest element found at position (" 
              << max_pos.first << ", " << max_pos.second << "): "
              << int_matrix[max_pos.first][max_pos.second] << std::endl;
    
    // Demonstrate matrices with other data types
    std::vector<std::vector<double>> double_matrix = {
        {3.14, 2.71, 1.618},
        {1.414, 2.236, 0.577},
        {1.732, 2.718, 3.141}
    };
    edulcni::matrix_widget("double_matrix", double_matrix);
    
    std::vector<std::vector<std::string>> string_matrix = {
        {"apple", "banana", "cherry"},
        {"orange", "grape", "lemon"},
        {"kiwi", "mango", "pear"}
    };
    edulcni::matrix_widget("string_matrix", string_matrix);
    
    // Highlight some elements
    edulcni::matrix_highlight("double_matrix", 0, 0);
    edulcni::matrix_highlight("string_matrix", 1, 1);
    
    std::cout << "Visualization exported to ./matrix_viz_output\n";
    std::cout << "Open ./matrix_viz_output/index.html in a browser to view\n";
    
    return 0;
} 