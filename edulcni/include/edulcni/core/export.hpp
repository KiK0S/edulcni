/**
 * Export system for edulcni
 */
#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include "edulcni/core/state.hpp"
#include "edulcni/core/json.hpp"

namespace edulcni {
namespace internal {

class Exporter {
public:
    void export_to_html(const std::string& output_dir, const std::vector<Frame>& frames) {
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(output_dir);
        
        // Export frames as JSON
        export_json(output_dir, frames);
        
        // Copy viewer files
        copy_viewer_files(output_dir);
    }
    
private:
    void export_json(const std::string& output_dir, const std::vector<Frame>& frames) {
        json::value frames_json = json::array{};
        
        for (const auto& frame : frames) {
            frames_json.as_array().push_back(frame.to_json());
        }
        
        json::value data = json::object{};
        data["frames"] = frames_json;
        
        std::ofstream json_file(output_dir + "/data.js");
        json_file << "const vizData = " << data.to_string() << ";" << std::endl;
        json_file.close();
    }
    
    void copy_viewer_files(const std::string& output_dir) {
        // In a real implementation, we would copy the HTML/CSS/JS files
        // from embedded resources or a known location
        
        // For now, we'll create minimal viewer files
        create_minimal_viewer(output_dir);
    }
    
    void create_minimal_viewer(const std::string& output_dir) {
        // Create CSS directory
        std::filesystem::create_directories(output_dir + "/css");
        
        // Create JS directories
        std::filesystem::create_directories(output_dir + "/js/widgets");
        
        // Write index.html
        std::ofstream html_file(output_dir + "/index.html");
        html_file << R"(<!DOCTYPE html>
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
    <script src="js/widgets/string_compare.js"></script>
    <script src="js/controls.js"></script>
    <script src="js/viewer.js"></script>
    <script src="data.js"></script>
    <script>
        const viewer = new VizViewer(vizData);
        viewer.initialize();
    </script>
</body>
</html>)";
        html_file.close();
        
        // Write CSS
        std::ofstream css_file(output_dir + "/css/styles.css");
        css_file << R"(
body {
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 0;
}

.viz-container {
    display: flex;
    flex-direction: column;
    height: 100vh;
}

.viz-header {
    padding: 10px;
    background-color: #f0f0f0;
    border-bottom: 1px solid #ddd;
}

.viz-main {
    display: flex;
    flex: 1;
    overflow: auto;
}

.viz-column {
    flex: 1;
    padding: 10px;
    border-right: 1px solid #ddd;
}

.viz-column:last-child {
    border-right: none;
}

.viz-controls {
    padding: 10px;
    background-color: #f0f0f0;
    border-top: 1px solid #ddd;
    display: flex;
    align-items: center;
}

.widget-container {
    margin-bottom: 20px;
    border: 1px solid #ddd;
    border-radius: 4px;
    overflow: hidden;
}

.widget-title {
    padding: 5px 10px;
    background-color: #f5f5f5;
    border-bottom: 1px solid #ddd;
    font-weight: bold;
}

.widget-content {
    padding: 10px;
}

/* Array widget styles */
.array-widget {
    display: flex;
    flex-wrap: wrap;
}

.array-cell {
    width: 30px;
    height: 30px;
    border: 1px solid #ddd;
    display: flex;
    align-items: center;
    justify-content: center;
    margin: 2px;
}

.array-cell.highlighted {
    background-color: #ffeb3b;
}

.array-cell.selected {
    background-color: #4caf50;
    color: white;
}

/* Button styles */
button {
    padding: 5px 10px;
    margin-right: 5px;
}

/* Slider styles */
input[type="range"] {
    margin: 0 10px;
    flex: 1;
}

/* Frame counter */
#frame-counter {
    min-width: 50px;
    text-align: right;
}
)";
        css_file.close();
        
        // Write viewer.js
        std::ofstream viewer_js_file(output_dir + "/js/viewer.js");
        viewer_js_file << R"(
class VizViewer {
    constructor(data) {
        this.data = data;
        this.currentFrame = 0;
        this.playing = false;
        this.playInterval = null;
        this.playSpeed = 1000; // ms between frames
        this.widgetRenderers = {
            'array': new ArrayWidgetRenderer(),
            'tree': new TreeWidgetRenderer(),
            // Add other widget renderers here
        };
    }
    
    initialize() {
        this.setupControls();
        this.updateFrameSlider();
        this.renderFrame(0);
    }
    
    setupControls() {
        document.getElementById('btn-prev').addEventListener('click', () => this.prevFrame());
        document.getElementById('btn-next').addEventListener('click', () => this.nextFrame());
        document.getElementById('btn-play').addEventListener('click', () => this.togglePlay());
        
        const slider = document.getElementById('frame-slider');
        slider.max = this.data.frames.length;
        slider.addEventListener('input', (e) => {
            this.renderFrame(parseInt(e.target.value - 1));
        });
    }
    
    updateFrameSlider() {
        const slider = document.getElementById('frame-slider');
        slider.max = this.data.frames.length;
        slider.value = this.currentFrame + 1;
        document.getElementById('frame-counter').textContent = 
            `${this.currentFrame + 1}/${this.data.frames.length}`;
    }
    
    renderFrame(frameIndex) {
        if (frameIndex < 0 || frameIndex >= this.data.frames.length) {
            return;
        }
        
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
        this.updateFrameSlider();
    }
    
    prevFrame() {
        if (this.currentFrame > 0) {
            this.renderFrame(this.currentFrame - 1);
        }
    }
    
    nextFrame() {
        if (this.currentFrame < this.data.frames.length - 1) {
            this.renderFrame(this.currentFrame + 1);
        } else {
            this.stopPlay();
        }
    }
    
    togglePlay() {
        if (this.playing) {
            this.stopPlay();
        } else {
            this.startPlay();
        }
    }
    
    startPlay() {
        if (!this.playing) {
            this.playing = true;
            document.getElementById('btn-play').textContent = 'Pause';
            this.playInterval = setInterval(() => {
                if (this.currentFrame < this.data.frames.length - 1) {
                    this.renderFrame(this.currentFrame + 1);
                } else {
                    this.stopPlay();
                }
            }, this.playSpeed);
        }
    }
    
    stopPlay() {
        if (this.playing) {
            this.playing = false;
            document.getElementById('btn-play').textContent = 'Play';
            clearInterval(this.playInterval);
            this.playInterval = null;
        }
    }
}
)";
        viewer_js_file.close();
        
        // Write controls.js
        std::ofstream controls_js_file(output_dir + "/js/controls.js");
        controls_js_file << R"(
// Additional control functionality can be added here
)";
        controls_js_file.close();
        
        // Write array.js
        std::ofstream array_js_file(output_dir + "/js/widgets/array.js");
        array_js_file << R"(
class ArrayWidgetRenderer {
    render(container, data) {
        // Render the array
        const arrayDiv = document.createElement('div');
        arrayDiv.className = 'array-widget';
        
        for (let i = 0; i < data.values.length; i++) {
            const cellDiv = document.createElement('div');
            cellDiv.className = 'array-cell';
            cellDiv.textContent = data.values[i];
            
            if (data.highlights && data.highlights.includes(i)) {
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
        const cell = document.querySelector(`#widget-${widgetId} .array-cell:nth-child(${index + 1})`);
        if (cell) {
            cell.classList.toggle('selected');
        }
    }
}
)";
        array_js_file.close();
        
        // Write tree.js
        std::ofstream tree_js_file(output_dir + "/js/widgets/tree.js");
        tree_js_file << R"(
class TreeWidgetRenderer {
    render(container, data) {
        // Render a simple tree for now
        const treeDiv = document.createElement('div');
        treeDiv.className = 'tree-widget';
        treeDiv.textContent = 'Tree visualization will be implemented here';
        
        container.appendChild(treeDiv);
    }
}
)";
        tree_js_file.close();
    }
};

} // namespace internal

// Public API for export
inline void export_visualization(const std::string& output_dir) {
    internal::State::instance().finalize();
    internal::Exporter exporter;
    exporter.export_to_html(output_dir, internal::State::instance().frames());
}

} // namespace edulcni 