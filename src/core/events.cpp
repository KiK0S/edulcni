#include "edulcni/core/events.hpp"
#include <string>
#include <functional>
#include <map>
#include <iostream>
#include <sstream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace edulcni {

namespace {
    // Stores callbacks indexed by event_id
    std::map<std::string, std::function<void()>> callbacks;
    std::map<std::string, std::vector<EventCallback>> event_callbacks; // Support multiple callbacks per event
    KeyCallback global_key_handler;
}

void register_callback(const std::string& event_id, std::function<void()> callback) {
    callbacks[event_id] = callback;
}

void register_event_callback(const std::string& event_id, EventCallback callback) {
    std::cout << "Registering event callback for: " << event_id << std::endl;
    event_callbacks[event_id].push_back(callback);
    std::cout << "Total callbacks for " << event_id << ": " << event_callbacks[event_id].size() << std::endl;
}

void register_click_handler(const std::string& widget_id, ClickCallback callback) {
    std::string event_id = widget_id + "_click";
    register_event_callback(event_id, [callback](const EventData& event) {
        callback(event.x, event.y);
    });
}

void register_key_handler(KeyCallback callback) {
    global_key_handler = callback;
}

// Helper function to parse JSON-like string (simple implementation)
EventData parse_event_data(const std::string& json_str) {
    EventData data;
    
    // Simple JSON parsing - in real implementation you'd use a proper JSON library
    // For now, we'll parse manually for the expected format:
    // {"type":"click","x":100,"y":50,"button":0,"target_id":"grid_id"}
    
    std::istringstream iss(json_str);
    std::string token;
    
    // This is a simplified parser - you might want to use nlohmann/json in production
    if (json_str.find("\"type\":\"click\"") != std::string::npos) {
        data.type = EventType::CLICK;
    } else if (json_str.find("\"type\":\"keypress\"") != std::string::npos) {
        data.type = EventType::KEY_PRESS;
    }
    
    // Extract x coordinate
    size_t x_pos = json_str.find("\"x\":");
    if (x_pos != std::string::npos) {
        x_pos += 4;
        size_t end_pos = json_str.find(",", x_pos);
        if (end_pos == std::string::npos) end_pos = json_str.find("}", x_pos);
        data.x = std::stoi(json_str.substr(x_pos, end_pos - x_pos));
    }
    
    // Extract y coordinate
    size_t y_pos = json_str.find("\"y\":");
    if (y_pos != std::string::npos) {
        y_pos += 4;
        size_t end_pos = json_str.find(",", y_pos);
        if (end_pos == std::string::npos) end_pos = json_str.find("}", y_pos);
        data.y = std::stoi(json_str.substr(y_pos, end_pos - y_pos));
    }
    
    // Extract button
    size_t button_pos = json_str.find("\"button\":");
    if (button_pos != std::string::npos) {
        button_pos += 9;
        size_t end_pos = json_str.find(",", button_pos);
        if (end_pos == std::string::npos) end_pos = json_str.find("}", button_pos);
        data.button = std::stoi(json_str.substr(button_pos, end_pos - button_pos));
    }
    
    // Extract target_id
    size_t target_pos = json_str.find("\"target_id\":\"");
    if (target_pos != std::string::npos) {
        target_pos += 13;
        size_t end_pos = json_str.find("\"", target_pos);
        data.target_id = json_str.substr(target_pos, end_pos - target_pos);
    }
    
    // Extract key
    size_t key_pos = json_str.find("\"key\":\"");
    if (key_pos != std::string::npos) {
        key_pos += 7;
        size_t end_pos = json_str.find("\"", key_pos);
        data.key = json_str.substr(key_pos, end_pos - key_pos);
    }
    
    return data;
}

namespace internal {

// C-style functions to be called from JavaScript
extern "C" {
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_KEEPALIVE
    void invoke_callback(const char* event_id) {
        std::cout << "invoke_callback: " << event_id << std::endl;
        std::string id(event_id);
        if (callbacks.find(id) != callbacks.end()) {
            callbacks[id]();
        }
    }
    
    EMSCRIPTEN_KEEPALIVE
    void invoke_callback_with_data(const char* event_id, const char* event_data_json) {
        std::cout << "invoke_callback_with_data: " << event_id << " " << event_data_json << std::endl;
        std::string id(event_id);
        std::string json_data(event_data_json);
        
        // Parse event data
        EventData event_data = parse_event_data(json_data);
        
        // Handle key events globally
        if (event_data.type == EventType::KEY_PRESS && global_key_handler) {
            global_key_handler(event_data.key);
            return;
        }
        
        // Handle widget-specific events
        std::cout << "Looking for event callback: " << id << std::endl;
        std::cout << "Available callbacks: ";
        for (const auto& pair : event_callbacks) {
            std::cout << pair.first << "(" << pair.second.size() << ") ";
        }
        std::cout << std::endl;
        
        auto it = event_callbacks.find(id);
        if (it != event_callbacks.end()) {
            std::cout << "Found " << it->second.size() << " callbacks for " << id << ", calling them" << std::endl;
            for (const auto& callback : it->second) {
                callback(event_data);
            }
        } else {
            std::cout << "No callback found for " << id << std::endl;
        }
    }

#else
    void invoke_callback(const char* event_id) {
        std::string id(event_id);
        if (callbacks.find(id) != callbacks.end()) {
            callbacks[id]();
        }
    }
    
    void invoke_callback_with_data(const char* event_id, const char* event_data_json) {
        std::string id(event_id);
        std::string json_data(event_data_json);
        
        // Parse event data
        EventData event_data = parse_event_data(json_data);
        
        // Handle key events globally
        if (event_data.type == EventType::KEY_PRESS && global_key_handler) {
            global_key_handler(event_data.key);
            return;
        }
        
        // Handle widget-specific events
        auto it = event_callbacks.find(id);
        if (it != event_callbacks.end()) {
            for (const auto& callback : it->second) {
                callback(event_data);
            }
        }
    }
#endif
}

} // namespace internal
} // namespace edulcni 