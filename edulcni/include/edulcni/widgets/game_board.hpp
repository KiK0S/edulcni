#ifndef EDULCNI_GAME_BOARD_HPP
#define EDULCNI_GAME_BOARD_HPP

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <any>
#include "edulcni/widgets/game_types.hpp"

namespace edulcni {

/**
 * Creates a game board widget with specified rows and columns
 * @param id unique identifier for the widget
 * @param rows number of rows
 * @param cols number of columns
 */
void create_game_board(const std::string& id, int rows, int cols);

/**
 * Updates the game board with new state
 * @param id identifier of the game board
 * @param board vector of cell states to display
 */
template <typename T>
void update_game_board(const std::string& id, const std::vector<T>& board);

/**
 * Registers a callback for cell clicks
 * @param id identifier of the game board
 * @param cell_index index of the cell (row * cols + col)
 * @param callback function to call when cell is clicked
 */
void register_cell_click_handler(const std::string& id, int cell_index, 
                                 std::function<void()> callback);

/**
 * Shows a message on the game board
 * @param id identifier of the game board
 * @param message text to display
 */
void show_game_message(const std::string& id, const std::string& message);

/**
 * Creates a button widget
 * @param id unique identifier for the button
 * @param label text to display on the button
 */
void create_button(const std::string& id, const std::string& label);

/**
 * Registers a callback for button clicks
 * @param id identifier of the button
 * @param callback function to call when button is clicked
 */
void register_button_click_handler(const std::string& id, 
                                   std::function<void()> callback);

/**
 * Introduces a delay in milliseconds
 * @param ms milliseconds to delay
 */
void delay(int ms);

/**
 * Starts the event loop for interactive applications
 * This will not return until the application is closed
 */
void start_event_loop();

} // namespace edulcni

#endif // EDULCNI_GAME_BOARD_HPP 