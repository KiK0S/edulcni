/**
 * JavaScript bridge for interactive edulcni applications
 */

// Ensure edulcniJS is initialized first
window.edulcniJS = window.edulcniJS || {};

// Map to store callback functions
const cellCallbacks = {};
const buttonCallbacks = {};

// Initialize the game board
window.edulcniJS.initGameBoard = function(id, rows, cols) {
    const board = document.getElementById(id);
    if (!board) return; // Skip if board element doesn't exist
    
    board.innerHTML = '';
    board.style.gridTemplateColumns = `repeat(${cols}, 100px)`;
    board.style.gridTemplateRows = `repeat(${rows}, 100px)`;
    
    for (let i = 0; i < rows * cols; i++) {
        const cell = document.createElement('div');
        cell.className = 'game-cell';
        cell.dataset.index = i;
        cell.addEventListener('click', () => {
            if (cellCallbacks[`${id}_${i}`]) {
                cellCallbacks[`${id}_${i}`]();
            }
        });
        board.appendChild(cell);
    }
    console.log(`Game board ${id} initialized with ${rows}x${cols} grid`);
};

// Update game board with new state
window.edulcniJS.updateGameBoard = function(id, boardState) {
    const board = document.getElementById(id);
    if (!board) return;
    
    const cells = board.querySelectorAll('.game-cell');
    
    for (let i = 0; i < cells.length; i++) {
        if (i >= boardState.length) break;
        
        switch (boardState[i]) {
            case 0: // Empty
                cells[i].textContent = '';
                break;
            case 1: // X
                cells[i].textContent = 'X';
                break;
            case 2: // O
                cells[i].textContent = 'O';
                break;
        }
    }
};

// Show game message
window.edulcniJS.showGameMessage = function(id, message) {
    const messageElement = document.getElementById('game-message');
    if (messageElement) {
        messageElement.textContent = message;
    }
};

// Register cell click callback
window.edulcniJS.registerCellClickHandler = function(id, cellIndex, callbackPtr) {
    cellCallbacks[`${id}_${cellIndex}`] = () => {
        Module.ccall('invoke_cell_callback', 'void', ['number'], [cellIndex], { async: true });
    };
};

// Register button click callback
window.edulcniJS.registerButtonClickHandler = function(id, callbackPtr) {
    const button = document.getElementById(id);
    if (button) {
        button.addEventListener('click', () => {
            Module.ccall('invoke_button_callback', 'void', ['string'], [id], { async: true });
        });
    }
};

console.log("edulcni_interactive.js loaded - JS interface ready"); 