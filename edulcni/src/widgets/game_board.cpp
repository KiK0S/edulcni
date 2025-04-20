#include "edulcni/widgets/game_board.hpp"
#include "edulcni/widgets/game_types.hpp"
#include "edulcni/core/state.hpp"
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/em_js.h>
#include <vector>
#include <string>
#include <sstream>

namespace edulcni {

// Store callbacks for WebAssembly to call
std::map<std::string, std::function<void()>> buttonCallbacks;
std::map<int, std::function<void()>> cellCallbacks;

// Game board state
struct GameBoardState {
    int rows;
    int cols;
    int cell_size;
    std::vector<CellState> cells;
    std::string message;
};

std::map<std::string, GameBoardState> boardStates;

// Helper JavaScript functions defined with EM_JS 
// These will be compiled to separate JavaScript functions

EM_JS(void, js_init_game_board, (const char* id, int rows, int cols), {
    window.edulcniJS.initGameBoard(UTF8ToString(id), rows, cols);
});

EM_JS(void, js_create_game_frame, (int rows, int cols, int cellSize), {
    if (!window.gameRenderer) return;
    
    // Create a new frame with empty commands array
    var frame = {
        commands: [],
        gameBoard: {
            rows: rows,
            cols: cols,
            cellSize: cellSize
        }
    };
    
    // Clear the canvas
    frame.commands.push({type: 'clear'});
    
    // Draw grid lines
    var width = rows * cellSize;
    var height = cols * cellSize;
    
    // Grid lines - horizontal
    for (var i = 0; i <= rows; i++) {
        var y = i * cellSize;
        frame.commands.push({
            type: 'drawLine', 
            x1: 0, 
            y1: y, 
            x2: width, 
            y2: y, 
            color: '#333', 
            width: 2
        });
    }
    
    // Grid lines - vertical
    for (var j = 0; j <= cols; j++) {
        var x = j * cellSize;
        frame.commands.push({
            type: 'drawLine', 
            x1: x, 
            y1: 0, 
            x2: x, 
            y2: height, 
            color: '#333', 
            width: 2
        });
    }
    
    // Add the frame to the renderer
    window.gameRenderer.addFrame(frame);
});

EM_JS(void, js_draw_x, (int x, int y, int size), {
    if (!window.gameRenderer) return;
    
    var frame = window.gameRenderer.frames[window.gameRenderer.frames.length - 1];
    if (!frame) return;
    
    // Draw X (two diagonal lines)
    frame.commands.push({
        type: 'drawLine',
        x1: x - size,
        y1: y - size,
        x2: x + size,
        y2: y + size,
        color: '#FF5252',
        width: 3
    });
    
    frame.commands.push({
        type: 'drawLine',
        x1: x + size,
        y1: y - size,
        x2: x - size,
        y2: y + size,
        color: '#FF5252',
        width: 3
    });
    
    window.gameRenderer.renderCurrentFrame();
});

EM_JS(void, js_draw_o, (int x, int y, int radius), {
    if (!window.gameRenderer) return;
    
    var frame = window.gameRenderer.frames[window.gameRenderer.frames.length - 1];
    if (!frame) return;
    
    // Draw O (circle)
    frame.commands.push({
        type: 'drawCircle',
        x: x,
        y: y,
        radius: radius,
        color: '#4CAF50',
        lineWidth: 3,
        fill: false
    });
    
    window.gameRenderer.renderCurrentFrame();
});

EM_JS(void, js_draw_message, (const char* message), {
    if (!window.gameRenderer) return;
    
    var frame = window.gameRenderer.frames[window.gameRenderer.frames.length - 1];
    if (!frame) return;
    
    // Add text message
    frame.commands.push({
        type: 'drawText',
        text: UTF8ToString(message),
        x: 225,
        y: 430,
        color: '#333',
        font: 'bold 24px Arial',
        align: 'center'
    });
    
    window.gameRenderer.renderCurrentFrame();
    
    // Also update message in DOM for compatibility
    var messageElement = document.getElementById('game-message');
    if (messageElement) {
        messageElement.textContent = UTF8ToString(message);
    }
});

EM_JS(void, js_register_cell_callback, (const char* id, int cellIndex, int callbackPtr), {
    window.edulcniJS.registerCellClickHandler(
        UTF8ToString(id), cellIndex, callbackPtr
    );
});

EM_JS(void, js_register_button_callback, (const char* id, int callbackPtr), {
    window.edulcniJS.registerButtonClickHandler(
        UTF8ToString(id), callbackPtr
    );
});

EM_JS(void, js_show_game_message, (const char* id, const char* message), {
    window.edulcniJS.showGameMessage(
        UTF8ToString(id), UTF8ToString(message)
    );
});

// Forward declare for compiler
void render_game_board(const std::string& id);

// Create a game board with specified dimensions
void create_game_board(const std::string& id, int rows, int cols) {
    // Calculate cell size based on a fixed canvas size of 450x450
    int cell_size = 450 / std::max(rows, cols);
    
    // Store the board state
    boardStates[id] = {
        rows,
        cols,
        cell_size,
        std::vector<CellState>(rows * cols, CellState::Empty),
        ""
    };
    
    // Draw the initial board
    render_game_board(id);
    
    // For compatibility with DOM-based version
    js_init_game_board(id.c_str(), rows, cols);
}

// Render the game board to canvas
void render_game_board(const std::string& id) {
    auto it = boardStates.find(id);
    if (it == boardStates.end()) return;
    
    const auto& state = it->second;
    
    // Create a new frame
    js_create_game_frame(state.rows, state.cols, state.cell_size);
    
    // Draw game pieces
    for (int i = 0; i < state.rows; i++) {
        for (int j = 0; j < state.cols; j++) {
            int index = i * state.cols + j;
            if (index >= state.cells.size()) continue;
            
            CellState cell = state.cells[index];
            if (cell != CellState::Empty) {
                int x = j * state.cell_size + state.cell_size / 2;
                int y = i * state.cell_size + state.cell_size / 2;
                int padding = state.cell_size * 0.2;
                
                if (cell == CellState::X) {
                    // Draw X
                    js_draw_x(x, y, state.cell_size/2 - padding);
                } else if (cell == CellState::O) {
                    // Draw O
                    js_draw_o(x, y, state.cell_size/2 - padding);
                }
            }
        }
    }
    
    // Draw message if any
    if (!state.message.empty()) {
        js_draw_message(state.message.c_str());
    }
    
    // Add to edulcni state tracking - Fixed namespace by fully qualifying it
    internal::State::instance().update_widget(id);
}

// Update the game board with new state
template <typename T>
void update_game_board(const std::string& id, const std::vector<T>& board) {
    // Update internal state
    auto it = boardStates.find(id);
    if (it == boardStates.end()) return;
    
    auto& state = it->second;
    state.cells.resize(board.size());
    
    for (size_t i = 0; i < board.size(); i++) {
        state.cells[i] = static_cast<CellState>(board[i]);
    }
    
    // Re-render the board
    render_game_board(id);
    
    // For compatibility with DOM-based version
    emscripten::val js_array = emscripten::val::array();
    for (size_t i = 0; i < board.size(); i++) {
        js_array.set(i, static_cast<int>(board[i]));
    }
    
    emscripten::val::global("edulcniJS").call<void>(
        "updateGameBoard", 
        emscripten::val(id), 
        js_array
    );
}

// Explicit template instantiations
template void update_game_board<int>(const std::string&, const std::vector<int>&);
template void update_game_board<CellState>(const std::string&, const std::vector<CellState>&);

// Register cell click handler
void register_cell_click_handler(const std::string& id, int cell_index, 
                               std::function<void()> callback) {
    cellCallbacks[cell_index] = callback;
    
    // For compatibility with DOM-based version
    js_register_cell_callback(id.c_str(), cell_index, reinterpret_cast<intptr_t>(&cellCallbacks[cell_index]));
}

// Show game message
void show_game_message(const std::string& id, const std::string& message) {
    auto it = boardStates.find(id);
    if (it == boardStates.end()) return;
    
    auto& state = it->second;
    state.message = message;
    
    // Re-render to show the message
    render_game_board(id);
    
    // For compatibility with DOM-based version
    js_show_game_message(id.c_str(), message.c_str());
}

// Create button widget
void create_button(const std::string& id, const std::string& label) {
    // Buttons are created in HTML
}

// Register button click handler
void register_button_click_handler(const std::string& id, 
                                 std::function<void()> callback) {
    buttonCallbacks[id] = callback;
    
    // For compatibility with DOM-based version
    js_register_button_callback(id.c_str(), reinterpret_cast<intptr_t>(&buttonCallbacks[id]));
}

// Add delay
void delay(int ms) {
    emscripten_sleep(ms);
}

// Start event loop
void start_event_loop() {
    emscripten_set_main_loop([](){}, 0, 1);
}

// Callback handlers exposed to JavaScript
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void invoke_cell_callback(int cell_index) {
        if (cellCallbacks.find(cell_index) != cellCallbacks.end()) {
            cellCallbacks[cell_index]();
        }
    }
    
    EMSCRIPTEN_KEEPALIVE
    void invoke_button_callback(const char* id) {
        std::string button_id(id);
        if (buttonCallbacks.find(button_id) != buttonCallbacks.end()) {
            buttonCallbacks[button_id]();
        }
    }
}

} // namespace edulcni