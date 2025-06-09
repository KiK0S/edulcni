// Sample frame data for the static viewer template
// Each function receives (ctx, canvas) parameters for drawing

window.frameData = [
  // Frame 1: Simple red rectangle
  function(ctx, canvas) {
    ctx.fillStyle = 'red';
    ctx.fillRect(50, 50, 100, 80);
  },

  // Frame 2: Blue circle
  function(ctx, canvas) {
    ctx.fillStyle = 'blue';
    ctx.beginPath();
    ctx.arc(150, 150, 40, 0, Math.PI * 2);
    ctx.fill();
  },

  // Frame 3: Green text
  function(ctx, canvas) {
    ctx.fillStyle = 'green';
    ctx.font = '24px Arial';
    ctx.fillText('Hello Canvas!', 100, 200);
  },

  // Frame 4: Multiple shapes
  function(ctx, canvas) {
    // Triangle
    ctx.fillStyle = 'purple';
    ctx.beginPath();
    ctx.moveTo(300, 50);
    ctx.lineTo(250, 150);
    ctx.lineTo(350, 150);
    ctx.closePath();
    ctx.fill();
    
    // Circle outline
    ctx.strokeStyle = 'orange';
    ctx.lineWidth = 3;
    ctx.beginPath();
    ctx.arc(400, 100, 30, 0, Math.PI * 2);
    ctx.stroke();
  },

  // Frame 5: Gradient background
  function(ctx, canvas) {
    const gradient = ctx.createLinearGradient(0, 0, canvas.width, 0);
    gradient.addColorStop(0, 'pink');
    gradient.addColorStop(1, 'lightblue');
    
    ctx.fillStyle = gradient;
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    
    ctx.fillStyle = 'white';
    ctx.font = '32px Arial';
    ctx.textAlign = 'center';
    ctx.fillText('Gradient Background!', canvas.width/2, canvas.height/2);
    ctx.textAlign = 'left'; // Reset
  },

  // Frame 6: Animation-like movement
  function(ctx, canvas) {
    ctx.fillStyle = 'darkblue';
    ctx.fillRect(100, 250, 60, 60);
    
    ctx.fillStyle = 'yellow';
    ctx.font = '16px Arial';
    ctx.fillText('Frame 6', 110, 280);
  },

  // Frame 7: Moving the rectangle
  function(ctx, canvas) {
    ctx.fillStyle = 'darkblue';
    ctx.fillRect(180, 250, 60, 60);
    
    ctx.fillStyle = 'yellow';
    ctx.font = '16px Arial';
    ctx.fillText('Frame 7', 190, 280);
  },

  // Frame 8: Continue movement
  function(ctx, canvas) {
    ctx.fillStyle = 'darkblue';
    ctx.fillRect(260, 250, 60, 60);
    
    ctx.fillStyle = 'yellow';
    ctx.font = '16px Arial';
    ctx.fillText('Frame 8', 270, 280);
  },

  // Frame 9: Complex drawing with paths
  function(ctx, canvas) {
    // Draw a star
    ctx.fillStyle = 'gold';
    ctx.beginPath();
    const centerX = 400;
    const centerY = 300;
    const spikes = 5;
    const outerRadius = 40;
    const innerRadius = 20;
    
    for (let i = 0; i < spikes * 2; i++) {
      const radius = i % 2 === 0 ? outerRadius : innerRadius;
      const angle = (i * Math.PI) / spikes;
      const x = centerX + Math.cos(angle) * radius;
      const y = centerY + Math.sin(angle) * radius;
      
      if (i === 0) {
        ctx.moveTo(x, y);
      } else {
        ctx.lineTo(x, y);
      }
    }
    ctx.closePath();
    ctx.fill();
    
    ctx.strokeStyle = 'darkorange';
    ctx.lineWidth = 2;
    ctx.stroke();
  },

  // Frame 10: Final frame with everything
  function(ctx, canvas) {
    // Background
    ctx.fillStyle = 'lightgray';
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    
    // Title
    ctx.fillStyle = 'black';
    ctx.font = 'bold 28px Arial';
    ctx.textAlign = 'center';
    ctx.fillText('Frame Viewer Demo Complete!', canvas.width/2, 50);
    
    // Some shapes
    ctx.fillStyle = 'red';
    ctx.fillRect(50, 100, 50, 50);
    
    ctx.fillStyle = 'blue';
    ctx.beginPath();
    ctx.arc(150, 125, 25, 0, Math.PI * 2);
    ctx.fill();
    
    ctx.fillStyle = 'green';
    ctx.beginPath();
    ctx.moveTo(250, 100);
    ctx.lineTo(225, 150);
    ctx.lineTo(275, 150);
    ctx.closePath();
    ctx.fill();
    
    // Reset text alignment
    ctx.textAlign = 'left';
  }
]; 