/**
 * Shared Library Demo - External C++ Project Example
 * 
 * This demonstrates how to use Edulcni as a shared library
 * in your own C++ projects for algorithm visualization.
 * 
 * Compile with:
 *   g++ -std=c++17 -I/path/to/edulcni/include shared_library_demo.cpp \
 *       -L/path/to/edulcni/lib -ledulcni -o shared_demo
 * 
 * Or after installation:
 *   g++ -std=c++17 -I$HOME/.local/include shared_library_demo.cpp \
 *       -L$HOME/.local/lib -ledulcni -o shared_demo
 */

#include <edulcni.hpp>
#include <edulcni/widgets/array.hpp>
#include <edulcni/widgets/matrix.hpp>
#include <edulcni/widgets/tree.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

// Example 1: Visualize bubble sort algorithm
void demonstrate_sorting() {
    std::cout << "=== Bubble Sort Visualization ===" << std::endl;
    
    // Initialize edulcni
    edulcni::initialize("shared_demo_output");
    
    // Create initial array
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 5};
    
    std::cout << "Initial array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    // Show initial state
    edulcni::array_widget("sort_visualization", arr);
    edulcni::console("Starting bubble sort...", "info");
    edulcni::step();
    
    // Bubble sort with visualization
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap elements
                std::swap(arr[j], arr[j + 1]);
                
                // Update visualization
                edulcni::array_widget("sort_visualization", arr);
                edulcni::console("Swapped " + std::to_string(arr[j]) + " and " + std::to_string(arr[j+1]), "log");
                edulcni::delay(300); // Slow down for visualization
                edulcni::step();
            }
        }
    }
    
    // Final sorted array
    edulcni::console("Sorting complete!", "success");
    edulcni::alert("Array sorted successfully!");
    edulcni::step();
    
    std::cout << "Final array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
}

// Example 2: Matrix operations visualization
void demonstrate_matrix_operations() {
    std::cout << "\n=== Matrix Operations Visualization ===" << std::endl;
    
    // Create a matrix for demonstration
    std::vector<std::vector<int>> matrix = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };
    
    std::cout << "Original matrix:" << std::endl;
    for (const auto& row : matrix) {
        for (int val : row) {
            std::cout << val << "\t";
        }
        std::cout << std::endl;
    }
    
    // Show original matrix
    edulcni::matrix_widget("matrix_demo", matrix);
    edulcni::console("Original 4x4 matrix", "info");
    edulcni::step();
    
    // Transpose the matrix step by step
    edulcni::console("Transposing matrix...", "info");
    std::vector<std::vector<int>> transposed(4, std::vector<int>(4));
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            transposed[j][i] = matrix[i][j];
            
            // Update visualization after each element
            edulcni::matrix_widget("matrix_demo", transposed);
            edulcni::console("Transposed element (" + std::to_string(i) + "," + std::to_string(j) + ")", "log");
            edulcni::delay(200);
            edulcni::step();
        }
    }
    
    edulcni::console("Matrix transposition complete!", "success");
    edulcni::alert("Matrix transposed successfully!");
    edulcni::step();
    
    std::cout << "Transposed matrix:" << std::endl;
    for (const auto& row : transposed) {
        for (int val : row) {
            std::cout << val << "\t";
        }
        std::cout << std::endl;
    }
}

// Example 3: Algorithm comparison
void demonstrate_search_algorithms() {
    std::cout << "\n=== Search Algorithm Comparison ===" << std::endl;
    
    std::vector<int> arr = {2, 5, 8, 12, 16, 23, 38, 45, 56, 67, 78};
    int target = 23;
    
    // Linear search visualization
    edulcni::array_widget("linear_search", arr);
    edulcni::console("Linear Search for " + std::to_string(target), "info");
    edulcni::step();
    
    int linear_comparisons = 0;
    for (int i = 0; i < arr.size(); i++) {
        linear_comparisons++;
        edulcni::console("Checking index " + std::to_string(i) + ": " + std::to_string(arr[i]), "log");
        edulcni::delay(400);
        edulcni::step();
        
        if (arr[i] == target) {
            edulcni::console("Found " + std::to_string(target) + " at index " + std::to_string(i), "success");
            edulcni::console("Linear search: " + std::to_string(linear_comparisons) + " comparisons", "info");
            break;
        }
    }
    
    edulcni::delay(1000);
    
    // Binary search visualization  
    edulcni::array_widget("binary_search", arr);
    edulcni::console("Binary Search for " + std::to_string(target), "info");
    edulcni::step();
    
    int left = 0, right = arr.size() - 1;
    int binary_comparisons = 0;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        binary_comparisons++;
        
        edulcni::console("Checking middle index " + std::to_string(mid) + ": " + std::to_string(arr[mid]), "log");
        edulcni::delay(500);
        edulcni::step();
        
        if (arr[mid] == target) {
            edulcni::console("Found " + std::to_string(target) + " at index " + std::to_string(mid), "success");
            edulcni::console("Binary search: " + std::to_string(binary_comparisons) + " comparisons", "info");
            break;
        } else if (arr[mid] < target) {
            left = mid + 1;
            edulcni::console("Target is greater, searching right half", "log");
        } else {
            right = mid - 1;
            edulcni::console("Target is smaller, searching left half", "log");
        }
        edulcni::step();
    }
    
    edulcni::alert("Linear: " + std::to_string(linear_comparisons) + " vs Binary: " + std::to_string(binary_comparisons) + " comparisons");
}

int main() {
    std::cout << "Edulcni Shared Library Demo" << std::endl;
    std::cout << "============================" << std::endl;
    
    try {
        // Run different algorithm demonstrations
        demonstrate_sorting();
        demonstrate_matrix_operations();
        demonstrate_search_algorithms();
        
        std::cout << "\n=== Demo Complete ===" << std::endl;
        std::cout << "Check the 'shared_demo_output' directory for generated HTML files!" << std::endl;
        std::cout << "Open the HTML files in your browser to see the visualizations." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 