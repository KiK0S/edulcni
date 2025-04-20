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
        // Create JS directory
        std::filesystem::create_directories(output_dir + "/js");
        
        // Write index.html
        write_html_template(output_dir);
        
        // Write universal_viewer.js
        write_universal_viewer_js(output_dir);
    }
    
    void write_html_template(const std::string& output_dir) {
        std::ofstream html_file(output_dir + "/index.html");
        
        html_file << R"(<!DOCTYPE html>
<html>
<head>
    <title>edulcni Visualization</title>
    <style>
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
            flex: 1;
            overflow: auto;
            padding: 10px;
        }
        
        #visualization-canvas {
            border: 1px solid #ddd;
            background-color: white;
        }
        
        .viz-controls {
            padding: 10px;
            background-color: #f0f0f0;
            border-top: 1px solid #ddd;
            display: flex;
            align-items: center;
        }
        
        .viz-controls button {
            margin-right: 10px;
            padding: 5px 10px;
            background-color: #4CAF50;
            border: none;
            color: white;
            cursor: pointer;
            border-radius: 4px;
        }
        
        .viz-controls button:hover {
            background-color: #45a049;
        }
        
        #frame-slider {
            flex: 1;
            margin: 0 10px;
        }
        
        #frame-counter {
            min-width: 60px;
            text-align: center;
        }
        
        h1 {
            margin: 0;
            font-size: 20px;
        }
    </style>
</head>
<body>
    <div class="viz-container">
        <div class="viz-header">
            <h1 id="frame-title">Frame 1</h1>
        </div>
        
        <div class="viz-main">
            <canvas id="visualization-canvas"></canvas>
        </div>
        
        <div class="viz-controls">
            <button id="btn-prev">◀ Previous</button>
            <button id="btn-play">▶ Play</button>
            <button id="btn-next">Next ▶</button>
            <input type="range" id="frame-slider" min="1" max="100" value="1">
            <span id="frame-counter">1/100</span>
        </div>
    </div>
    
    <script src="js/universal_viewer.js"></script>
    <script src="data.js"></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            const viewer = new UniversalViewer(vizData);
            viewer.initialize();
        });
    </script>
</body>
</html>)";
        
        html_file.close();
    }
    
    void write_universal_viewer_js(const std::string& output_dir) {
        std::ofstream js_file(output_dir + "/js/universal_viewer.js");
        
        js_file << R"(/**
 * Universal Viewer for edulcni
 * This viewer can render any visualization based on rendering instructions
 */

class UniversalViewer {
    constructor(data) {
        this.data = data;
        this.currentFrame = 0;
        this.playing = false;
        this.playInterval = null;
        this.playSpeed = 1000; // 1 second per frame
        this.canvas = null;
        this.ctx = null;
    }
    
    initialize() {
        this.setupCanvas();
        this.setupControls();
        this.renderCurrentFrame();
    }
    
    setupCanvas() {
        this.canvas = document.getElementById('visualization-canvas');
        this.ctx = this.canvas.getContext('2d');
        
        // Set canvas size
        this.canvas.width = this.canvas.parentElement.clientWidth;
        this.canvas.height = 600;
        
        // Handle window resize
        window.addEventListener('resize', () => {
            this.canvas.width = this.canvas.parentElement.clientWidth;
            this.renderCurrentFrame();
        });
    }
    
    setupControls() {
        document.getElementById('btn-prev').addEventListener('click', () => this.prevFrame());
        document.getElementById('btn-play').addEventListener('click', () => this.togglePlay());
        document.getElementById('btn-next').addEventListener('click', () => this.nextFrame());
        
        const slider = document.getElementById('frame-slider');
        slider.min = 1;
        slider.max = this.data.frames.length;
        slider.value = 1;
        
        slider.addEventListener('input', () => {
            this.renderFrame(parseInt(slider.value) - 1);
        });
    }
    
    renderCurrentFrame() {
        this.renderFrame(this.currentFrame);
    }
    
    renderFrame(frameIndex) {
        if (frameIndex < 0 || frameIndex >= this.data.frames.length) {
            return;
        }
        
        const frame = this.data.frames[frameIndex];
        this.currentFrame = frameIndex;
        
        // Update UI
        document.getElementById('frame-title').textContent = `Frame ${frameIndex + 1}`;
        document.getElementById('frame-slider').value = frameIndex + 1;
        document.getElementById('frame-counter').textContent = `${frameIndex + 1}/${this.data.frames.length}`;
        
        // Clear canvas
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        
        // Render each widget
        let y = 20; // Starting y position for widgets
        for (const [widgetId, render] of Object.entries(frame.renders)) {
            // Add widget title
            this.ctx.font = "16px Arial";
            this.ctx.fillStyle = "#000";
            this.ctx.textAlign = "left";
            this.ctx.fillText(widgetId, 10, y);
            
            // Render widget with offset
            this.renderElement(render, 10, y + 5);
            
            // Calculate height for next widget
            const height = this.calculateElementHeight(render);
            y += height + 30; // Add space between widgets
        }
    }
    
    calculateElementHeight(element) {
        switch (element.type) {
            case 'group':
                let maxHeight = 0;
                for (const child of element.children) {
                    const childHeight = this.calculateElementHeight(child);
                    maxHeight = Math.max(maxHeight, childHeight);
                }
                return maxHeight + element.y;
                
            case 'rect':
                return element.height + element.y;
                
            case 'text':
                return 20 + element.y; // Approximate text height
                
            case 'line':
                return Math.max(element.y1, element.y2);
                
            case 'circle':
                return element.radius * 2 + element.y;
                
            default:
                return 50; // Default height
        }
    }
    
    renderElement(element, offsetX = 0, offsetY = 0) {
        this.ctx.save();
        
        switch (element.type) {
            case 'group':
                this.ctx.translate(offsetX + element.x, offsetY + element.y);
                for (const child of element.children) {
                    this.renderElement(child);
                }
                break;
                
            case 'rect':
                this.ctx.fillStyle = element.fill;
                this.ctx.strokeStyle = element.stroke;
                this.ctx.lineWidth = element.strokeWidth;
                
                this.ctx.fillRect(
                    offsetX + element.x, 
                    offsetY + element.y, 
                    element.width, 
                    element.height
                );
                
                this.ctx.strokeRect(
                    offsetX + element.x, 
                    offsetY + element.y, 
                    element.width, 
                    element.height
                );
                break;
                
            case 'text':
                this.ctx.font = element.font;
                this.ctx.fillStyle = element.fill;
                this.ctx.textAlign = element.textAlign;
                
                this.ctx.fillText(
                    element.text, 
                    offsetX + element.x, 
                    offsetY + element.y
                );
                break;
                
            case 'line':
                this.ctx.strokeStyle = element.stroke;
                this.ctx.lineWidth = element.strokeWidth;
                
                this.ctx.beginPath();
                this.ctx.moveTo(offsetX + element.x1, offsetY + element.y1);
                this.ctx.lineTo(offsetX + element.x2, offsetY + element.y2);
                this.ctx.stroke();
                break;
                
            case 'circle':
                this.ctx.fillStyle = element.fill;
                this.ctx.strokeStyle = element.stroke;
                this.ctx.lineWidth = element.strokeWidth;
                
                this.ctx.beginPath();
                this.ctx.arc(
                    offsetX + element.x, 
                    offsetY + element.y, 
                    element.radius, 
                    0, 
                    Math.PI * 2
                );
                this.ctx.fill();
                this.ctx.stroke();
                break;
        }
        
        this.ctx.restore();
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
            document.getElementById('btn-play').textContent = '▶ Play';
            clearInterval(this.playInterval);
            this.playInterval = null;
        }
    }
})";
        
        js_file.close();
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