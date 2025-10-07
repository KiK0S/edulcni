#include "edulcni.hpp"
#include <iostream>
#include <chrono>
#include <thread>

using namespace edulcni;

// Wrapper functions that capture visualization at the right moments
int factorial_with_visualization(int n) {
    RECURSION_STACK_FRAME("factorial", 
                         "n = " + std::to_string(n));
    
    if (n <= 1) {
        return 1;
    }
    
    int result = n * factorial_with_visualization(n - 1);
    return result;
}

int fibonacci_with_visualization(int n) {
    RECURSION_STACK_FRAME("fibonacci", 
                         "n = " + std::to_string(n));
    
    if (n <= 1) {
        return n;
    }
    
    int result = fibonacci_with_visualization(n - 1) + fibonacci_with_visualization(n - 2);
    return result;
}

int main() {
    // Initialize edulcni
    initialize("./recursion_viz_output");
    
    // Create a recursion widget
    auto recursion_id = create_recursion_widget("factorial", RecursionStackMode::CurrentStack);
    
    
    std::cout << "Recursion Widget Demo" << std::endl;
    std::cout << "=====================" << std::endl;
    
    // Demo 1: Current Stack Mode (default)
    std::cout << "\n1. Current Stack Mode - Factorial(5):" << std::endl;
    
    // Capture the stack during recursion by calling step() at the right moment
    int result1 = factorial_with_visualization(5);
    std::cout << "Result: " << result1 << std::endl;
    
    step();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // Demo 2: Full Tree Mode
    std::cout << "\n2. Full Tree Mode - Fibonacci(4):" << std::endl;
    recursion_id = create_recursion_widget("fibonacci", RecursionStackMode::FullTree);

    int result2 = fibonacci_with_visualization(4);
    std::cout << "Result: " << result2 << std::endl;
    
    step();
    recursion_clear("fibonacci");
    delete_widget("fibonacci");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // Demo 3: Current Stack Mode with Fibonacci
    std::cout << "\n3. Current Stack Mode - Fibonacci(3):" << std::endl;
    recursion_id = create_recursion_widget("fibonacci", RecursionStackMode::CurrentStack);
    
    int result3 = fibonacci_with_visualization(3);
    std::cout << "Result: " << result3 << std::endl;
    
    std::cout << "\nVisualization generated! Check the output files." << std::endl;
    
    return 0;
}
