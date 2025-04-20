/**
 * Canvas-based renderer for edulcni visualizations
 * Receives drawing commands from C++ backend
 */

class CanvasRenderer {
  constructor(canvasId, width = 500, height = 500) {
    this.canvas = document.getElementById(canvasId);
    if (!this.canvas) {
      console.error(`Canvas element with id '${canvasId}' not found`);
      return;
    }
    
    this.canvas.width = width;
    this.canvas.height = height;
    this.ctx = this.canvas.getContext('2d');
    this.frames = [];
    this.currentFrame = 0;
    
    // Set up click handling
    this.canvas.addEventListener('click', this.handleCanvasClick.bind(this));
    
    // Setup frame navigation controls
    this.setupControls();
  }
  
  // Handle canvas click
  handleCanvasClick(event) {
    // Only process clicks when viewing the latest frame
    if (this.currentFrame !== this.frames.length - 1) {
      this.goToFrame(this.frames.length - 1);
      return;
    }
    
    const rect = this.canvas.getBoundingClientRect();
    const x = event.clientX - rect.left;
    const y = event.clientY - rect.top;
    
    // If current frame has a game board, calculate which cell was clicked
    const frame = this.frames[this.currentFrame];
    if (frame && frame.gameBoard) {
      const { rows, cols, cellSize } = frame.gameBoard;
      
      const row = Math.floor(y / cellSize);
      const col = Math.floor(x / cellSize);
      
      if (row >= 0 && row < rows && col >= 0 && col < cols) {
        const cellIndex = row * cols + col;
        // Call C++ callback for this cell
        Module.ccall('invoke_cell_callback', 'void', ['number'], [cellIndex], { async: true });
      }
    }
  }
  
  // Set up control buttons
  setupControls() {
    const controls = [
      { id: 'first-btn', action: () => this.goToFrame(0), label: '⏮️' },
      { id: 'prev-btn', action: () => this.goToFrame(this.currentFrame - 1), label: '⏪' },
      { id: 'next-btn', action: () => this.goToFrame(this.currentFrame + 1), label: '⏩' },
      { id: 'last-btn', action: () => this.goToFrame(this.frames.length - 1), label: '⏭️' }
    ];
    
    const container = document.createElement('div');
    container.className = 'canvas-controls';
    container.style.display = 'flex';
    container.style.justifyContent = 'center';
    container.style.margin = '10px 0';
    
    controls.forEach(control => {
      const button = document.createElement('button');
      button.id = control.id;
      button.innerHTML = control.label;
      button.addEventListener('click', control.action);
      button.style.margin = '0 5px';
      container.appendChild(button);
    });
    
    // Add frame counter
    this.frameCounter = document.createElement('div');
    this.frameCounter.className = 'frame-counter';
    this.frameCounter.style.margin = '10px 0';
    this.frameCounter.textContent = 'Frame: 0/0';
    
    // Insert controls after canvas
    this.canvas.parentNode.insertBefore(container, this.canvas.nextSibling);
    this.canvas.parentNode.insertBefore(this.frameCounter, container.nextSibling);
    
    // Also add canvas at the bottom of the page to match the button that already exists
    const resetButton = document.getElementById('reset_button');
    if (resetButton) {
      resetButton.addEventListener('click', () => {
        Module.ccall('invoke_button_callback', 'void', ['string'], ['reset_button'], { async: true });
      });
    }
  }
  
  // Navigate to a specific frame
  goToFrame(frameIndex) {
    if (frameIndex < 0 || frameIndex >= this.frames.length) {
      return;
    }
    
    this.currentFrame = frameIndex;
    this.renderCurrentFrame();
    this.updateFrameCounter();
  }
  
  // Add a new frame with drawing commands
  addFrame(frameData) {
    this.frames.push(frameData);
    this.currentFrame = this.frames.length - 1;
    this.renderCurrentFrame();
    this.updateFrameCounter();
  }
  
  // Update the frame counter display
  updateFrameCounter() {
    if (this.frameCounter) {
      this.frameCounter.textContent = `Frame: ${this.currentFrame + 1}/${this.frames.length}`;
    }
  }
  
  // Render the current frame
  renderCurrentFrame() {
    if (this.frames.length === 0) return;
    
    const frame = this.frames[this.currentFrame];
    this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
    
    // Process all drawing commands in the frame
    frame.commands.forEach(cmd => {
      switch (cmd.type) {
        case 'clear':
          this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
          break;
          
        case 'drawLine':
          this.ctx.beginPath();
          this.ctx.moveTo(cmd.x1, cmd.y1);
          this.ctx.lineTo(cmd.x2, cmd.y2);
          this.ctx.strokeStyle = cmd.color || '#000';
          this.ctx.lineWidth = cmd.width || 1;
          this.ctx.stroke();
          break;
          
        case 'drawRect':
          if (cmd.fill) {
            this.ctx.fillStyle = cmd.color || '#000';
            this.ctx.fillRect(cmd.x, cmd.y, cmd.width, cmd.height);
          } else {
            this.ctx.strokeStyle = cmd.color || '#000';
            this.ctx.lineWidth = cmd.lineWidth || 1;
            this.ctx.strokeRect(cmd.x, cmd.y, cmd.width, cmd.height);
          }
          break;
          
        case 'drawCircle':
          this.ctx.beginPath();
          this.ctx.arc(cmd.x, cmd.y, cmd.radius, 0, 2 * Math.PI);
          if (cmd.fill) {
            this.ctx.fillStyle = cmd.color || '#000';
            this.ctx.fill();
          } else {
            this.ctx.strokeStyle = cmd.color || '#000';
            this.ctx.lineWidth = cmd.lineWidth || 1;
            this.ctx.stroke();
          }
          break;
          
        case 'drawText':
          this.ctx.font = cmd.font || '16px Arial';
          this.ctx.fillStyle = cmd.color || '#000';
          this.ctx.textAlign = cmd.align || 'left';
          this.ctx.fillText(cmd.text, cmd.x, cmd.y);
          break;
      }
    });
  }
}

// Expose the renderer to the global scope
window.CanvasRenderer = CanvasRenderer;

// Create renderer when document is loaded
document.addEventListener('DOMContentLoaded', () => {
  if (document.getElementById('game-canvas')) {
    window.gameRenderer = new CanvasRenderer('game-canvas');
  }
}); 