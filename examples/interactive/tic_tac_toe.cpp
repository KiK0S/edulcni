/**
 * Interactive Tic Tac Toe game using edulcni
 */
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <algorithm>
#include "edulcni.hpp"
#include "edulcni/widgets/grid.hpp"
#include "edulcni/widgets/button.hpp"
#include "edulcni/core/widget.hpp"

// Define game-specific cell types
enum class TicTacToeCell {
    Empty = 0,
    X = 1,
    O = 2
};

// Game state
std::vector<TicTacToeCell> board(9, TicTacToeCell::Empty);
bool player_turn = true; // true for X, false for O

// Helper function to convert board state to a 2D matrix for display
std::vector<std::vector<char>> board_to_matrix() {
    std::vector<std::vector<char>> matrix(3, std::vector<char>(3, '.'));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int index = i * 3 + j;
            switch (board[index]) {
                case TicTacToeCell::Empty:
                    matrix[i][j] = '.';
                    break;
                case TicTacToeCell::X:
                    matrix[i][j] = 'X';
                    break;
                case TicTacToeCell::O:
                    matrix[i][j] = 'O';
                    break;
            }
        }
    }
    return matrix;
}

// Check if game is over and return winner (Empty = draw or ongoing)
TicTacToeCell check_winner() {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i*3] != TicTacToeCell::Empty && 
            board[i*3] == board[i*3+1] && 
            board[i*3] == board[i*3+2])
            return board[i*3];
    }
    
    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board[i] != TicTacToeCell::Empty && 
            board[i] == board[i+3] && 
            board[i] == board[i+6])
            return board[i];
    }
    
    // Check diagonals
    if (board[0] != TicTacToeCell::Empty && 
        board[0] == board[4] && 
        board[0] == board[8])
        return board[0];
    
    if (board[2] != TicTacToeCell::Empty && 
        board[2] == board[4] && 
        board[2] == board[6])
        return board[2];
    
    // Check for draw
    for (auto cell : board) {
        if (cell == TicTacToeCell::Empty)
            return TicTacToeCell::Empty; // Game still ongoing
    }
    
    return TicTacToeCell::Empty; // Draw
}

// Make AI move (simple implementation)
void make_ai_move() {
    // Find first empty cell
    for (int i = 0; i < 9; i++) {
        if (board[i] == TicTacToeCell::Empty) {
            board[i] = TicTacToeCell::O;
            break;
        }
    }
    player_turn = true;
}

// Update the visual representation of the board
void update_board_display() {
    std::cout << "Updating board display" << std::endl;
    // Update the 2D matrix representation
    edulcni::matrix_widget("tictactoe", board_to_matrix());
    
    // Render the frame
    edulcni::step();
}

// Handle cell click
void cell_click_handler(int cell_index) {
    std::cout << "CLICK EVENT RECEIVED! Cell index: " << cell_index << std::endl;
    edulcni::console("Click received on cell " + std::to_string(cell_index), "log");
    
    // If cell is already taken or game is over, ignore click
    if (board[cell_index] != TicTacToeCell::Empty || !player_turn || 
        check_winner() != TicTacToeCell::Empty) {
        std::cout << "Click ignored - cell taken, not player turn, or game over" << std::endl;
        return;
    }
    
    // Update board with player's move
    board[cell_index] = TicTacToeCell::X;
    
    // Update display
    update_board_display();
    
    // Check for winner
    TicTacToeCell winner = check_winner();
    if (winner != TicTacToeCell::Empty) {
        std::string message = (winner == TicTacToeCell::X) ? "You win!" : "Computer wins!";
        edulcni::alert(message);
        return;
    }
    
    // Check for draw
    bool is_draw = true;
    for (auto cell : board) {
        if (cell == TicTacToeCell::Empty) {
            is_draw = false;
            break;
        }
    }
    
    if (is_draw) {
        edulcni::alert("Draw!");
        return;
    }
    
    // AI's turn
    player_turn = false;
    
    // Add small delay for AI "thinking"
    edulcni::delay(500); // 500ms delay
    
    make_ai_move();
    update_board_display();
    
    // Check for winner again
    winner = check_winner();
    if (winner != TicTacToeCell::Empty) {
        std::string message = (winner == TicTacToeCell::X) ? "You win!" : "Computer wins!";
        edulcni::alert(message);
    } else if (std::all_of(board.begin(), board.end(), 
               [](TicTacToeCell s) { return s != TicTacToeCell::Empty; })) {
        edulcni::alert("Draw!");
    }
}

// Reset game
void reset_game() {
    for (auto& cell : board) {
        cell = TicTacToeCell::Empty;
    }
    player_turn = true;
    
    // Clear the board
    edulcni::Grid::clear("tictactoe");
    edulcni::alert("game reset");
    edulcni::step();
}

// Helper function for registering cell handlers
void register_cell_handlers() {
    std::cout << "Registering cell handlers..." << std::endl;
    edulcni::console("Registering click handlers for 9 cells", "log");
    
    // Register click handlers for each cell
    for (int i = 0; i < 9; i++) {
        int row = i / 3;
        int col = i % 3;
        
        std::cout << "Registering handler for cell " << i << " (row=" << row << ", col=" << col << ")" << std::endl;
        
        // Create a lambda capture to pass cell index to the handler
        auto handler = [i]() {
            std::cout << "Lambda called for cell " << i << std::endl;
            cell_click_handler(i);
        };
        
        edulcni::Grid::register_cell_handler("tictactoe", i, handler);
    }
    std::cout << "Finished registering cell handlers" << std::endl;
}

int main() {
    // Initialize edulcni
    edulcni::initialize("./tictactoe_output");
    
    // Enable interactive mode for browser interaction
    edulcni::enable_interactive_mode();
    
    // Create grid widget for the game board
    edulcni::Grid::create("tictactoe", 3, 3);
    
    // Register cell handlers
    register_cell_handlers();
    
    // Create reset button widget
    edulcni::button_widget("reset_button", "Reset Game", reset_game);
    
    // Position widgets on the canvas
    edulcni::center_widget("tictactoe", 800, 600);  // Center the grid
    edulcni::position_widget_bottom_center("reset_button", 800, 600, 80);  // Button at bottom
    
    // Initial board render
    update_board_display();
    
    // Start event loop
    edulcni::start_interactive_loop();
    
    return 0;
} 