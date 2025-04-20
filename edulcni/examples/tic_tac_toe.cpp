/**
 * Interactive Tic Tac Toe game using edulcni
 */
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <algorithm>
#include "edulcni.hpp"
#include "edulcni/widgets/game_types.hpp"

using edulcni::CellState;

// Game state
std::vector<CellState> board(9, CellState::Empty);
bool player_turn = true; // true for X, false for O

// Check if game is over and return winner (Empty = draw or ongoing)
CellState check_winner() {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i*3] != CellState::Empty && 
            board[i*3] == board[i*3+1] && 
            board[i*3] == board[i*3+2])
            return board[i*3];
    }
    
    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board[i] != CellState::Empty && 
            board[i] == board[i+3] && 
            board[i] == board[i+6])
            return board[i];
    }
    
    // Check diagonals
    if (board[0] != CellState::Empty && 
        board[0] == board[4] && 
        board[0] == board[8])
        return board[0];
    
    if (board[2] != CellState::Empty && 
        board[2] == board[4] && 
        board[2] == board[6])
        return board[2];
    
    // Check for draw
    for (auto cell : board) {
        if (cell == CellState::Empty)
            return CellState::Empty; // Game still ongoing
    }
    
    return CellState::Empty; // Draw
}

// Make AI move (simple implementation)
void make_ai_move() {
    // Find first empty cell
    for (int i = 0; i < 9; i++) {
        if (board[i] == CellState::Empty) {
            board[i] = CellState::O;
            break;
        }
    }
    player_turn = true;
}

// Handle cell click
void cell_click_handler(int cell_index) {
    // If cell is already taken or game is over, ignore click
    if (board[cell_index] != CellState::Empty || !player_turn || 
        check_winner() != CellState::Empty)
        return;
    
    // Update board with player's move
    board[cell_index] = CellState::X;
    
    // Update display
    edulcni::update_game_board("tictactoe", board);
    
    // Check for winner
    CellState winner = check_winner();
    if (winner != CellState::Empty) {
        std::string message = (winner == CellState::X) ? "You win!" : "Computer wins!";
        edulcni::show_game_message("tictactoe", message);
        return;
    }
    
    // Check for draw
    bool is_draw = true;
    for (auto cell : board) {
        if (cell == CellState::Empty) {
            is_draw = false;
            break;
        }
    }
    
    if (is_draw) {
        edulcni::show_game_message("tictactoe", "Draw!");
        return;
    }
    
    // AI's turn
    player_turn = false;
    
    // Add small delay for AI "thinking"
    edulcni::delay(500); // 500ms delay
    
    make_ai_move();
    edulcni::update_game_board("tictactoe", board);
    
    // Check for winner again
    winner = check_winner();
    if (winner != CellState::Empty) {
        std::string message = (winner == CellState::X) ? "You win!" : "Computer wins!";
        edulcni::show_game_message("tictactoe", message);
    } else if (std::all_of(board.begin(), board.end(), 
               [](CellState s) { return s != CellState::Empty; })) {
        edulcni::show_game_message("tictactoe", "Draw!");
    }
}

// Reset game
void reset_game() {
    for (auto& cell : board) {
        cell = CellState::Empty;
    }
    player_turn = true;
    edulcni::update_game_board("tictactoe", board);
    edulcni::show_game_message("tictactoe", "");
}

int main() {
    // Initialize edulcni
    edulcni::initialize("./tictactoe_output", true); // true for interactive mode
    
    // Create game board widget
    edulcni::create_game_board("tictactoe", 3, 3);
    
    // Register click handlers for each cell
    for (int i = 0; i < 9; i++) {
        edulcni::register_cell_click_handler("tictactoe", i, 
            std::bind(cell_click_handler, i));
    }
    
    // Add reset button
    edulcni::create_button("reset_button", "Reset Game");
    edulcni::register_button_click_handler("reset_button", reset_game);
    
    // Start game loop - this will keep the application running
    // and processing events until the window is closed
    edulcni::start_event_loop();
    
    return 0;
} 