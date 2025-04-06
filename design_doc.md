# edulcni: A Competitive Programming Visualization Library

## Design Document

This document outlines the architecture and implementation plan for `edulcni`, a C++ library designed to help competitive programmers visualize their algorithms and data structures.

## 1. Overall Architecture

The proposed system consists of four main components:

┌───────────────────┐      ┌───────────────────┐      ┌───────────────────┐
│  C++ Core Library │ ──→  │  Export System    │ ──→  │  Web Viewer       │
│  (edulcni)        │      │  (JSON/HTML/WASM) │      │  (Browser-based)  │
└───────────────────┘      └───────────────────┘      └───────────────────┘
         ↑                                                     ↑
         │                                                     │
         └─────────────────────────────────────────────────────
                       (Optional interactive mode)

1.1 Core Library (C++)
The core library will be a header-only C++ library that competitive programmers can include in their solutions. It will provide:
* Widget-based visualization functions (arrays, trees, graphs, etc.)
* State management (auto/manual frame creation)
* Data capture and serialization

1.2 Export System
The export system will:
* Serialize captured visualization data to JSON
* Generate standalone HTML/JS for viewing
* Support WASM compilation for interactive applications

1.3 Web Viewer
A browser-based viewer that:
* Displays visualization frames as a slideshow
* Provides navigation controls
* Supports interactive elements when needed

## 2. Core Library

### 2.1 Header Structure

```cpp
// edulcni.hpp (main include)
#pragma once

#include "edulcni/core/state.hpp"
#include "edulcni/core/frame.hpp"
#include "edulcni/core/export.hpp"

#include "edulcni/widgets/array.hpp"
#include "edulcni/widgets/tree.hpp"
#include "edulcni/widgets/graph.hpp"
#include "edulcni/widgets/plane.hpp"
#include "edulcni/widgets/accumulator.hpp"
// ... other widgets

namespace edulcni {
    // Public API functions
    void initialize(const std::string& output_dir = "./viz_output");
    void finalize();
    void breakpoint();
    void freeze();
    void unfreeze();
    
    // Widget functions (examples)
    template<typename Iterator>
    void array_widget(const std::string& id, Iterator begin, Iterator end);
    void array_highlight(const std::string& id, int index);
    
    // ... other widget functions
}
```

### 2.2 State Management
```cpp
namespace edulcni {
namespace internal {

class State {
private:
    std::vector<Frame> frames_;
    std::map<std::string, std::unique_ptr<Widget>> widgets_;
    bool frozen_ = false;
    std::string output_dir_;
    
public:
    static State& instance() {
        static State state;
        return state;
    }
    
    void initialize(const std::string& output_dir);
    void finalize();
    void breakpoint();
    void freeze();
    void unfreeze();
    
    template<typename WidgetT, typename... Args>
    WidgetT* get_or_create_widget(const std::string& id, Args&&... args);
    
    void update_widget(const std::string& id);
};

} // namespace internal
} // namespace edulcni
```

### 2.3 Widget base class

```cpp
namespace edulcni {
namespace internal {

class Widget {
protected:
    std::string id_;
    
public:
    Widget(std::string id) : id_(std::move(id)) {}
    virtual ~Widget() = default;
    
    virtual json::value to_json() const = 0;
    virtual void update_from_json(const json::value& data) = 0;
    virtual std::string widget_type() const = 0;
};

} // namespace internal
} // namespace edulcni
```

### 2.4 Example Widget Implementation: Array

```cpp
namespace edulcni {
namespace internal {

class ArrayWidget : public Widget {
private:
    std::vector<std::variant<int, double, std::string>> data_;
    std::unordered_set<int> highlights_;
    
public:
    ArrayWidget(std::string id) : Widget(std::move(id)) {}
    
    template<typename Iterator>
    void update(Iterator begin, Iterator end) {
        data_.clear();
        std::transform(begin, end, std::back_inserter(data_),
                      [](const auto& val) { return val; });
        highlights_.clear();
    }
    
    void highlight(int index) {
        highlights_.insert(index);
    }
    
    json::value to_json() const override {
        // Implementation details
    }
    
    void update_from_json(const json::value& data) override {
        // Implementation details
    }
    
    std::string widget_type() const override {
        return "array";
    }
};

} // namespace internal

// Public API
template<typename Iterator>
void array_widget(const std::string& id, Iterator begin, Iterator end) {
    auto* widget = internal::State::instance().get_or_create_widget<internal::ArrayWidget>(id);
    widget->update(begin, end);
    internal::State::instance().update_widget(id);
}

void array_highlight(const std::string& id, int index) {
    auto* widget = internal::State::instance().get_or_create_widget<internal::ArrayWidget>(id);
    widget->highlight(index);
    internal::State::instance().update_widget(id);
}

} // namespace edulcni
```

## 3. Building a Slideshow App

The slideshow app will be a browser-based viewer for visualizations:

```
web/
├── js/
│   ├── viewer.js         - Core viewer functionality
│   ├── widgets/          - Widget renderers
│   └── controls.js       - Navigation controls
├── css/
│   └── styles.css        - Visual styling
└── index.html            - Main viewer page
```

### 3.1 HTML structure

```html
<!DOCTYPE html>
<html>
<head>
    <title>edulcni Visualization</title>
    <link rel="stylesheet" href="css/styles.css">
</head>
<body>
    <div class="viz-container">
        <div class="viz-header">
            <h1 id="frame-title">Frame 1</h1>
        </div>
        
        <div class="viz-main">
            <div class="viz-column" id="column-1"></div>
            <div class="viz-column" id="column-2"></div>
            <div class="viz-column" id="column-3"></div>
        </div>
        
        <div class="viz-controls">
            <button id="btn-prev">Previous</button>
            <button id="btn-play">Play</button>
            <button id="btn-next">Next</button>
            <input type="range" id="frame-slider" min="1" max="100" value="1">
            <span id="frame-counter">1/100</span>
        </div>
    </div>
    
    <script src="js/widgets/array.js"></script>
    <script src="js/widgets/tree.js"></script>
    <!-- Other widget renderers -->
    
    <script src="js/controls.js"></script>
    <script src="js/viewer.js"></script>
    <script>
        // Load visualization data
        const vizData = /* JSON data from export */;
        const viewer = new VizViewer(vizData);
        viewer.initialize();
    </script>
</body>
</html>
```

### 3.2 JavaScript Viewer

```js
// viewer.js
class VizViewer {
    constructor(data) {
        this.data = data;
        this.currentFrame = 0;
        this.playing = false;
        this.playInterval = null;
        this.widgetRenderers = {
            'array': new ArrayWidgetRenderer(),
            'tree': new TreeWidgetRenderer(),
            // Other widget renderers
        };
    }
    
    initialize() {
        this.setupControls();
        this.renderFrame(0);
    }
    
    renderFrame(frameIndex) {
        const frame = this.data.frames[frameIndex];
        document.getElementById('frame-title').textContent = `Frame ${frameIndex + 1}`;
        
        // Clear columns
        for (let i = 1; i <= 3; i++) {
            document.getElementById(`column-${i}`).innerHTML = '';
        }
        
        // Render each widget
        for (const [widgetId, widgetData] of Object.entries(frame.widgets)) {
            const type = widgetData.type;
            const renderer = this.widgetRenderers[type];
            
            if (renderer) {
                const containerDiv = document.createElement('div');
                containerDiv.className = 'widget-container';
                containerDiv.id = `widget-${widgetId}`;
                
                const titleDiv = document.createElement('div');
                titleDiv.className = 'widget-title';
                titleDiv.textContent = widgetId;
                containerDiv.appendChild(titleDiv);
                
                const contentDiv = document.createElement('div');
                contentDiv.className = 'widget-content';
                containerDiv.appendChild(contentDiv);
                
                // Determine which column to place the widget in
                const column = widgetData.column || 1;
                document.getElementById(`column-${column}`).appendChild(containerDiv);
                
                // Render the widget
                renderer.render(contentDiv, widgetData);
            }
        }
        
        this.currentFrame = frameIndex;
        document.getElementById('frame-slider').value = frameIndex + 1;
        document.getElementById('frame-counter').textContent = `${frameIndex + 1}/${this.data.frames.length}`;
    }
    
    // Navigation methods and control setup...
}
```

## 4. Adding Callbacks for Interactivity

### 4.1 C++ Callback Registration

```cpp
namespace edulcni {

// Define callback types
using MouseCallback = std::function<void(int x, int y, int button)>;
using KeyCallback = std::function<void(int key)>;

// Widget-specific callbacks
void array_on_click(const std::string& id, std::function<void(int index)> callback);
void tree_on_click(const std::string& id, std::function<void(int node_id)> callback);

namespace internal {
    // Store callbacks in widget classes
    class WidgetWithCallbacks : public Widget {
    protected:
        std::map<std::string, std::function<void(json::value)>> callbacks_;
        
    public:
        void register_callback(const std::string& event, std::function<void(json::value)> callback) {
            callbacks_[event] = std::move(callback);
        }
        
        json::value callbacks_to_json() const {
            json::value result = json::object();
            for (const auto& [event, _] : callbacks_) {
                result[event] = true;
            }
            return result;
        }
        
        // Method to process callback from web viewer
        void process_callback(const std::string& event, const json::value& data) {
            auto it = callbacks_.find(event);
            if (it != callbacks_.end()) {
                it->second(data);
            }
        }
    };
}

} // namespace edulcni
```

### 4.2 JavaScript Event Handling

```js
// In widget renderers
class ArrayWidgetRenderer {
    render(container, data) {
        // Render the array
        const arrayDiv = document.createElement('div');
        arrayDiv.className = 'array-widget';
        
        for (let i = 0; i < data.values.length; i++) {
            const cellDiv = document.createElement('div');
            cellDiv.className = 'array-cell';
            cellDiv.textContent = data.values[i];
            
            if (data.highlights.includes(i)) {
                cellDiv.classList.add('highlighted');
            }
            
            // Add click event if callback is registered
            if (data.callbacks && data.callbacks.click) {
                cellDiv.addEventListener('click', () => {
                    this.handleClick(data.id, i);
                });
                cellDiv.classList.add('clickable');
            }
            
            arrayDiv.appendChild(cellDiv);
        }
        
        container.appendChild(arrayDiv);
    }
    
    handleClick(widgetId, index) {
        // If using WASM, call back into C++
        if (window.edulcniModule) {
            window.edulcniModule.processCallback(widgetId, 'click', {index: index});
        }
        
        // For static visualization, just highlight
        document.querySelector(`#widget-${widgetId} .array-cell:nth-child(${index + 1})`)
            .classList.toggle('selected');
    }
}
```


## 5. Exporting to a Static Website

### 5.1 C++ Export Functionality

```cpp
namespace edulcni {
namespace internal {

class Exporter {
public:
    void export_to_html(const std::string& output_dir, const std::vector<Frame>& frames) {
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(output_dir);
        
        // Export frames as JSON
        std::ofstream json_file(output_dir + "/data.js");
        json_file << "const vizData = " << frames_to_json(frames) << ";";
        json_file.close();
        
        // Copy HTML/CSS/JS files
        copy_viewer_files(output_dir);
    }
    
private:
    json::value frames_to_json(const std::vector<Frame>& frames) {
        // Implementation details
    }
    
    void copy_viewer_files(const std::string& output_dir) {
        // Copy necessary files from embedded resources
    }
};

} // namespace internal

// Public API
void export_visualization(const std::string& output_dir = "./viz_output") {
    internal::State::instance().finalize();
    internal::Exporter exporter;
    exporter.export_to_html(output_dir, internal::State::instance().frames());
}

} // namespace edulcni
```
### 5.2 HTML Launcher

```html
<!DOCTYPE html>
<html>
<head>
    <title>edulcni Visualization</title>
    <link rel="stylesheet" href="css/styles.css">
</head>
<body>
    <div class="viz-container">
        <!-- Visualization UI -->
    </div>
    
    <!-- Widget renderers -->
    <script src="js/widgets/array.js"></script>
    <script src="js/widgets/tree.js"></script>
    
    <!-- Main viewer -->
    <script src="data.js"></script>
    <script src="js/controls.js"></script>
    <script src="js/viewer.js"></script>
    <script>
        const viewer = new VizViewer(vizData);
        viewer.initialize();
    </script>
</body>
</html>
```


## 6. WASM Integration for Interactive Applications
 
### 6.1 Emscripten Setup
For interactive applications, we can compile the visualization with Emscripten:

```cpp
// wasm_main.cpp
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include "edulcni.hpp"

// State for the interactive application
namespace app {
    // Application-specific state
}

// JavaScript-callable functions
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void initialize() {
        edulcni::initialize();
        // Initialize application state
    }
    
    EMSCRIPTEN_KEEPALIVE
    void process_input(int type, int x, int y) {
        // Handle input and update visualization
    }
    
    EMSCRIPTEN_KEEPALIVE
    void process_callback(const char* widget_id, const char* event, const char* data) {
        // Process callback from widget
    }
}

// Emscripten bindings
EMSCRIPTEN_BINDINGS(edulcni_module) {
    emscripten::function("initialize", &initialize);
    emscripten::function("processInput", &process_input);
    emscripten::function("processCallback", &process_callback);
}
```

### 6.2 Compilation Command

```bash
emcc -O2 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 \
    -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
    -s EXPORTED_FUNCTIONS='["_initialize", "_process_input", "_process_callback", "_malloc", "_free"]' \
    -I. wasm_main.cpp -o viz.js
```