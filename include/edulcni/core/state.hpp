/**
 * State management for edulcni
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include "edulcni/core/frame.hpp"
#include "edulcni/core/export.hpp"
#include "edulcni/core/widget.hpp"
#include "edulcni/core/layout.hpp"
#include "edulcni/core/js_bindings.hpp"
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

namespace edulcni {
namespace internal {

class State {
private:
    std::vector<Frame> frames_;
    std::map<std::string, std::unique_ptr<Widget>> widgets_;
    std::string output_dir_;
    bool initialized_ = false;
    LayoutManager layout_manager_;
    
    // Private constructor for singleton
    State() = default;
    
    void _assert_initialized() {
        if (!initialized_)
            throw std::runtime_error("edulcni not initialized. Call edulcni::initialize() first.");
    }

    ~State() {
        Exporter exporter;
        exporter.export_to_html(output_dir_, frames_);
    }

public:
    // Singleton access
    static State& instance() {
        static State state;
        return state;
    }
    
    void initialize(const std::string& output_dir) {
        if (initialized_)
            return;
        
        output_dir_ = output_dir;
        frames_.clear();
        widgets_.clear();
        initialized_ = true;
    }
    
    void step() {
        _assert_initialized();

        // Apply auto-layout if enabled
        if (layout_manager_.config().strategy != LayoutStrategy::Free) {
            auto layout = layout_manager_.calculate_layout(widgets_);
            layout_manager_.apply_layout(layout, widgets_);
        }

        // Create a new frame with current widget states and renders
        Frame frame;
        for (const auto& [id, widget] : widgets_) {
            if (!widget->is_contained()) {
                // Get the widget's render and position it
                auto render = widget->render();
                render->to_right(widget->x());
                render->to_bottom(widget->y());
                frame.add_widget_state(id, std::move(render));
            }
        }
        
        frames_.push_back(std::move(frame));
    
        #ifdef __EMSCRIPTEN__
            render_latest_frame();
        #endif
    }
    
    template<typename WidgetT, typename... Args>
    WidgetT* get_or_create_widget(const std::string& id, Args&&... args) {
        _assert_initialized();
        
        auto it = widgets_.find(id);
        if (it != widgets_.end()) {
            // Widget exists, return it after casting
            WidgetT* casted = dynamic_cast<WidgetT*>(it->second.get());
            if (!casted)
                throw std::runtime_error("Widget with ID " + id + " already exists, but is of a different type.");
            return casted;
        }
        
        // Create new widget
        auto widget = std::make_unique<WidgetT>(id, std::forward<Args>(args)...);
        WidgetT* result = widget.get();
        widgets_[id] = std::move(widget);
        return result;
    }
    
    // Get widget by base type (for common operations)
    template<typename WidgetBaseT>
    WidgetBaseT* get_widget_typed(const std::string& id) {
        _assert_initialized();
        
        auto it = widgets_.find(id);
        if (it != widgets_.end()) {
            return dynamic_cast<WidgetBaseT*>(it->second.get());
        }

        return nullptr;
    }

    Widget* get_widget(const std::string& id) {
        _assert_initialized();
        
        auto it = widgets_.find(id);
        if (it != widgets_.end()) {
            return it->second.get();
        }

        return nullptr;
    }
    
    void delete_widget(const std::string& id) {
        _assert_initialized();
        
        auto it = widgets_.find(id);
        if (it != widgets_.end()) {
            widgets_.erase(it);
        }
    }
    
    const std::vector<Frame>& frames() const {
        return frames_;
    }
    
    const std::string& output_dir() const {
        return output_dir_;
    }
    
    LayoutManager& layout_manager() {
        return layout_manager_;
    }
    
    const LayoutManager& layout_manager() const {
        return layout_manager_;
    }

public:
    void alert(const std::string& message) {
        js_alert(message.c_str());
    }
    void delay(int ms) {
        js_delay(ms);
    }
    void console(const std::string& message, const std::string& type) {
        js_console(message.c_str(), type.c_str());
    }

#ifdef __EMSCRIPTEN__
    // Renders the latest frame in interactive mode
    void render_latest_frame() {
        if (frames_.empty()) {
            return;
        }
        
        // Get the latest frame and convert it to JavaScript function
        const Frame& latest_frame = frames_.back();
        std::string frame_js = latest_frame.to_canvas_js();
        
        // Call JavaScript to append this frame function
        EM_ASM({
            var frameJs = UTF8ToString($0);
            var frameFunction;
            
            try {
                // Evaluate the JavaScript function
                frameFunction = eval("(" + frameJs + ")");
                
                // Append to frameData if it exists, or notify viewer
                if (window.appendFrame && typeof window.appendFrame === 'function') {
                    window.appendFrame(frameFunction);
                } else if (window.edulcni && window.edulcni.onFrameReady) {
                    window.edulcni.onFrameReady(frameFunction);
                }
            } catch (error) {
                console.error('Error evaluating frame function:', error);
            }
        }, frame_js.c_str());
    }
#endif

};

} // namespace internal

} // namespace edulcni 