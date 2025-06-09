# Edulcni - Competitive Programming Visualization Framework

A modern C++ visualization framework designed for competitive programming and algorithm education. Edulcni enables you to create beautiful, interactive visualizations of your algorithms with minimal code changes.

## Features

### 🎯 **Dual Build Modes**
- **Static Mode**: Generate standalone HTML files with step-by-step algorithm visualization
- **Interactive Mode**: Real-time browser-based visualization with user interaction capabilities  
- **Shared Library**: Build as a shared library for easy integration into existing C++ projects

### 🧩 **Rich Widget System**
- **Grid/Matrix**: 2D grid visualizations perfect for DP problems, game boards, and matrices
- **Array**: Linear data structure visualization for sorting, searching algorithms
- **Tree**: Binary tree and general tree structure visualization
- **Graph**: Graph traversal algorithms (DFS, BFS) with visual highlighting
- **Binary Mask**: Bit manipulation visualization
- **Mixed Types**: Compare different data structures side-by-side

### ⚡ **Real-time Interaction**
- Click handlers for grid cells and interactive elements  
- Keyboard event support
- Real-time state updates
- Custom event callbacks

### 🎨 **Advanced Visualization**
- Step-by-step algorithm execution
- Customizable colors and styling
- Delay/timing control
- Console logging and alerts
- Automatic frame capture

## Quick Start

### Installation Requirements

#### For Static Mode (Default)
- `g++` compiler with C++17 support
- Standard build tools (make)

#### For Interactive Mode  
- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) installed and activated
- Web browser for running examples

#### For Shared Library
- `g++` compiler with C++17 support
- Standard build tools (make)

### Building the Framework

```bash
# Clone the repository
git clone <repository-url>
cd edulcni

# Build static library (default)
make static

# Build interactive library  
make interactive

# Build shared library
make shared

# Build all examples
make examples

# Clean build files
make clean
```

### Your First Visualization

```cpp
#include "edulcni.hpp"
#include "edulcni/widgets/array.hpp"

int main() {
    // Initialize the framework
    edulcni::initialize("output");
    
    // Create and visualize an array
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90};
    edulcni::array_widget("sort_demo", arr);
    edulcni::step();
    
    // Bubble sort with visualization
    for (int i = 0; i < arr.size() - 1; i++) {
        for (int j = 0; j < arr.size() - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                
                // Update visualization
                edulcni::array_widget("sort_demo", arr);
                edulcni::delay(500); // 500ms delay
                edulcni::step();
            }
        }
    }
    
    return 0;
}
```

### Using as Shared Library

#### Recommended Pattern for Competitive Programming

The recommended approach uses conditional compilation to enable visualization only in debug mode:

```cpp
// your_solution.cpp
#include <iostream>
#include <vector>
#include <algorithm>

#ifdef DEBUG
#include <edulcni.hpp>
#endif

using namespace std;

// Conditional compilation macros
#ifdef DEBUG
    #define EDULCNI_CALL(x) edulcni::x
#else
    #define EDULCNI_CALL(x)
#endif

int main() {
    // Initialize framework (only in DEBUG mode)
    EDULCNI_CALL(initialize("my_solution"));
    
    int n;
    cin >> n;
    vector<int> arr(n);
    
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }
    
    // Visualize array (only in DEBUG mode)
    EDULCNI_CALL(array_widget("arr", arr));
    EDULCNI_CALL(step());
    
    // Your algorithm with visualization
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                
                // Update visualization (only in DEBUG mode)
                EDULCNI_CALL(array_widget("arr", arr));
                EDULCNI_CALL(array_highlight("arr", j));
                EDULCNI_CALL(array_highlight("arr", j + 1));
                EDULCNI_CALL(step());
                EDULCNI_CALL(array_clear_highlights("arr"));
            }
        }
    }
    
    return 0;
}
```

#### Working with C-Style Arrays

Edulcni supports both `std::vector` and C-style arrays:

```cpp
#ifdef DEBUG
#include <edulcni.hpp>
#endif

// Conditional compilation macro
#ifdef DEBUG
    #define EDULCNI_CALL(x) edulcni::x
#else
    #define EDULCNI_CALL(x)
#endif

const int MAXN = 100;
int arr[MAXN];
int n;

int main() {
    EDULCNI_CALL(initialize("array_demo"));
    
    cin >> n;
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }
    
    // Visualize C-style array using pointer arithmetic
    EDULCNI_CALL(array_widget("arr", arr, arr + n));
    EDULCNI_CALL(step());
    
    // Highlight specific elements
    EDULCNI_CALL(array_highlight("arr", 0));
    EDULCNI_CALL(array_highlight("arr", n-1));
    EDULCNI_CALL(step());
    
    // Clear highlights
    EDULCNI_CALL(array_clear_highlights("arr"));
    EDULCNI_CALL(step());
    
    return 0;
}
```

#### Compilation Commands

```bash
# Debug build with visualization
g++ -std=c++17 -DDEBUG -I/path/to/edulcni/include your_solution.cpp -L/path/to/edulcni -ledulcni -o debug_solution

# Release build without visualization (for submission)
g++ -std=c++17 your_solution.cpp -o release_solution

# Or using a Makefile approach
# Debug: make debug
# Release: make release
```

#### Advanced Usage Example

```cpp
#ifdef DEBUG
#include <edulcni.hpp>
#endif

#ifdef DEBUG
    #define EDULCNI_CALL(x) edulcni::x
#else
    #define EDULCNI_CALL(x)
#endif

int main() {
    EDULCNI_CALL(initialize("advanced_demo"));
    
    vector<int> data = {5, 2, 8, 1, 9};
    
    // Create widgets
    EDULCNI_CALL(array_widget("original", data));
    EDULCNI_CALL(label_widget("status", "Starting sort..."));
    EDULCNI_CALL(step());
    
    // Algorithm with detailed visualization
    for (int i = 0; i < data.size() - 1; i++) {
        EDULCNI_CALL(label_widget("pass", "Pass " + to_string(i + 1)));
        
        for (int j = 0; j < data.size() - i - 1; j++) {
            // Highlight comparison
            EDULCNI_CALL(array_highlight("original", j));
            EDULCNI_CALL(array_highlight("original", j + 1));
            EDULCNI_CALL(label_widget("comparing", 
                "Comparing " + to_string(data[j]) + " and " + to_string(data[j+1])));
            EDULCNI_CALL(step());
            
            if (data[j] > data[j + 1]) {
                swap(data[j], data[j + 1]);
                EDULCNI_CALL(array_widget("original", data));
                EDULCNI_CALL(label_widget("action", "Swapped!"));
                EDULCNI_CALL(step());
            }
            
            EDULCNI_CALL(array_clear_highlights("original"));
        }
    }
    
    EDULCNI_CALL(label_widget("status", "Sort complete!"));
    EDULCNI_CALL(step());
    
    return 0;
}
```

## Build Modes Explained

### Static Mode
Perfect for algorithm analysis and educational content:
- Generates complete HTML files with all visualization frames
- No server required - open HTML files directly in browser
- Ideal for sharing algorithm walkthroughs and tutorials

```bash
make static
make examples
# Open examples/static/*.html in your browser
```

### Interactive Mode  
Real-time interaction with your algorithms:
- Live updates as user interacts with visualization
- Event-driven programming model
- Perfect for games, interactive demos, and learning tools

```bash
make interactive  
make examples
# Serve examples/interactive/ directory with HTTP server
python3 -m http.server 8000
# Open http://localhost:8000/examples/interactive/
```

### Shared Library Mode
Integrate visualization into existing C++ projects:
- Build once, use anywhere
- Minimal overhead for existing codebases
- Standard C++ linking

```bash
make shared
# Library files created in lib/ directory
# Headers available in include/ directory
```

## Available Examples

### Static Examples
- `array_example` - Array sorting visualization
- `matrix_example` - 2D matrix operations  
- `graph_dfs_example` - Depth-first search visualization
- `tree_example` - Binary tree operations
- `std_sort_example` - STL sort algorithm visualization
- `binary_mask_example` - Bit manipulation visualization
- `mixed_types_example` - Multiple data structure comparison
- `compare_combined_example` - Algorithm comparison

### Interactive Examples  
- `tic_tac_toe` - Playable tic-tac-toe game with AI

### Building Examples

```bash
# Build all examples for current mode
make examples

# Build specific example
make array_example           # Static mode
make tic_tac_toe            # Current mode (static or interactive)

# Build examples for specific mode
make BUILD_MODE=static examples
make BUILD_MODE=interactive examples
```

### Running Examples

#### Static Examples
```bash
# After building static examples
cd examples/static
open array_example.html    # macOS
xdg-open array_example.html # Linux  
# Or drag and drop into browser
```

#### Interactive Examples
```bash
# After building interactive examples
cd examples/interactive
python3 -m http.server 8000
# Open browser to http://localhost:8000/tic_tac_toe.html
```

## API Reference

### Core Functions

```cpp
// Initialize framework
edulcni::initialize(const std::string& output_dir);

// Capture current frame  
edulcni::step();

// Add delays between steps
edulcni::delay(int milliseconds);

// Show alert dialog
edulcni::alert(const std::string& message);

// Log to console
edulcni::console(const std::string& message, const std::string& type);
```

### Widget Functions

```cpp
// Array visualization (std::vector)
edulcni::array_widget(const std::string& id, const std::vector<T>& data);

// Array visualization (C-style array with pointers)
edulcni::array_widget(const std::string& id, T* begin, T* end);

// Array highlighting
edulcni::array_highlight(const std::string& id, int index);
edulcni::array_clear_highlights(const std::string& id);

// Label/text widgets
edulcni::label_widget(const std::string& id, const std::string& text);
edulcni::label_widget(const std::string& id, int value);

// Matrix visualization  
edulcni::matrix_widget(const std::string& id, const std::vector<std::vector<T>>& matrix);

// Grid visualization
edulcni::grid_widget(const std::string& id, int rows, int cols);

// Tree visualization
edulcni::tree_widget(const std::string& id, /* tree parameters */);

// Graph visualization  
edulcni::graph_widget(const std::string& id, /* graph parameters */);
```

### Interactive Functions (Interactive Mode Only)

```cpp
// Enable interactive mode
edulcni::enable_interactive_mode();

// Register event handlers
edulcni::register_click_handler(const std::string& widget_id, ClickCallback callback);
edulcni::register_key_handler(KeyCallback callback);

// Control rendering
edulcni::set_auto_render(bool enabled);
edulcni::request_frame_update();

// Start interactive loop
edulcni::start_interactive_loop();
```

## Development

### Debug Builds

```bash
# Debug build for current mode
make debug

# Interactive debug build with enhanced debugging
make BUILD_MODE=interactive debug

# Debug specific example
make debug_tic_tac_toe
```

### Project Structure

```
edulcni/
├── include/           # Public headers
│   ├── edulcni.hpp   # Main API header
│   └── edulcni/      # Individual widget headers
├── src/              # Source code
│   ├── core/         # Core framework
│   └── widgets/      # Widget implementations  
├── examples/         # Example programs
├── static/           # Static HTML templates
├── build/            # Build artifacts
└── lib/              # Shared library output (when built)
```

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)  
5. Open a Pull Request

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Built with modern C++17
- Interactive mode powered by [Emscripten](https://emscripten.org/)
- Designed for competitive programming and algorithm education
- Inspired by the need for better algorithm visualization tools 