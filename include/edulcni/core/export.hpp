/**
 * Export system for edulcni
 */
#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <cmath>
#include "edulcni/core/frame.hpp"

namespace edulcni {
namespace internal {

class Exporter {
public:
    void export_to_html(const std::string& output_dir, const std::vector<Frame>& frames) {
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(output_dir);
        
        // Export frames as JavaScript
        export_frame_data(output_dir, frames);
        
        // Copy viewer files using static templates
        copy_viewer_files(output_dir);
    }
    
private:
    void export_frame_data(const std::string& output_dir, const std::vector<Frame>& frames) {
        std::ofstream js_file(output_dir + "/data.js");

        render::Bounds global_bounds = render::Bounds::empty();
        for (const auto& frame : frames) {
            global_bounds.expand(frame.bounds());
        }

        const double margin = 40.0;
        double width = std::max(800.0, global_bounds.width() + margin * 2.0);
        double height = std::max(600.0, global_bounds.height() + margin * 2.0);
        double offset_x = global_bounds.is_empty() ? 0.0 : (global_bounds.min_x - margin);
        double offset_y = global_bounds.is_empty() ? 0.0 : (global_bounds.min_y - margin);

        js_file << "window.frameData = {\n";
        js_file << "    canvas: {\n";
        js_file << "        width: " << static_cast<int>(std::ceil(width)) << ",\n";
        js_file << "        height: " << static_cast<int>(std::ceil(height)) << ",\n";
        js_file << "        offsetX: " << offset_x << ",\n";
        js_file << "        offsetY: " << offset_y << "\n";
        js_file << "    },\n";
        js_file << "    frames: [\n";

        for (size_t i = 0; i < frames.size(); ++i) {
            if (i > 0) js_file << ",\n";
            js_file << "        " << frames[i].to_canvas_js();
        }

        js_file << "\n    ]\n";
        js_file << "};\n";
        js_file.close();
    }
    
    void copy_viewer_files(const std::string& output_dir) {
        // Find the static templates directory
        std::string template_dir = find_static_templates_dir();
        
        if (template_dir.empty()) {
            throw std::runtime_error("Could not find static templates directory");
        }
        
        // Write index.html using static template
        write_html_from_template(output_dir, template_dir + "/static_viewer_template.html");
    }
    
    void write_html_from_template(const std::string& output_dir, const std::string& template_path) {
        // Read the template file
        std::ifstream template_file(template_path);
        if (!template_file.is_open()) {
            throw std::runtime_error("Could not open template file: " + template_path);
        }
        
        std::string template_content((std::istreambuf_iterator<char>(template_file)),
                                   std::istreambuf_iterator<char>());
        template_file.close();
        
        // Replace placeholders
        replace_all(template_content, "$TITLE", "edulcni Visualization");
        replace_all(template_content, "$DATA_FILE_PATH", "data.js");
        
        // Write the final HTML file
        std::ofstream html_file(output_dir + "/index.html");
        html_file << template_content;
        html_file.close();
    }
    
    std::string find_static_templates_dir() {
        // Common locations to search for static templates
        std::vector<std::string> search_paths = {
            "./static/templates",
            "../static/templates", 
            "../../static/templates",
            "./share/edulcni/templates",
            "/usr/share/edulcni/templates",
            "/usr/local/share/edulcni/templates"
        };
        
        for (const auto& path : search_paths) {
            if (std::filesystem::exists(path + "/static_viewer_template.html")) {
                return path;
            }
        }
        
        return ""; // Not found
    }
    
    void replace_all(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }
};

} // namespace internal

} // namespace edulcni 