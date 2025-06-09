#pragma once

#include <functional>
#include <string>
#include <map>

namespace edulcni {

// Event types for browser interactions
enum class EventType {
    CLICK,
    KEY_PRESS
};

// Event data structure passed from browser
struct EventData {
    EventType type;
    int x, y;           // Mouse coordinates
    int button;         // Mouse button (0=left, 1=middle, 2=right)
    std::string key;    // Key name for keyboard events
    std::string target_id;  // Widget ID that triggered the event
};

// Callback types
using EventCallback = std::function<void(const EventData&)>;
using ClickCallback = std::function<void(int x, int y)>;
using KeyCallback = std::function<void(const std::string& key)>;

/**
 * Registers a callback function with a specific event ID
 * @param event_id Unique identifier for the event
 * @param callback Function to call when the event is triggered
 */
void register_callback(const std::string& event_id, std::function<void()> callback);

/**
 * Registers a callback function with event data
 * @param event_id Unique identifier for the event
 * @param callback Function to call when the event is triggered
 */
void register_event_callback(const std::string& event_id, EventCallback callback);

/**
 * Registers a click handler for a specific widget
 * @param widget_id Widget identifier
 * @param callback Function to call when widget is clicked
 */
void register_click_handler(const std::string& widget_id, ClickCallback callback);

/**
 * Registers a global key handler
 * @param callback Function to call when key is pressed
 */
void register_key_handler(KeyCallback callback);

namespace internal {
    // Invokes a callback based on its ID (called from JavaScript)
    void invoke_callback(const std::string& event_id);
    
    // Invokes a callback with event data (called from JavaScript)
    void invoke_callback_with_data(const std::string& event_id, const std::string& event_data_json);
}

} // namespace edulcni 