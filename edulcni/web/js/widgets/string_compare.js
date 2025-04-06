class StringCompareWidgetRenderer {
    render(container, data) {
        const compareDiv = document.createElement('div');
        compareDiv.className = 'string-compare-widget';
        
        // Create the first string display
        const str1Div = document.createElement('div');
        str1Div.className = 'string-row';
        
        for (let i = 0; i < data.str1.length; i++) {
            const charDiv = document.createElement('div');
            charDiv.className = 'string-char';
            charDiv.textContent = data.str1.charAt(i);
            
            if (i === data.pos1) {
                charDiv.classList.add('highlighted');
            }
            
            str1Div.appendChild(charDiv);
        }
        
        // Create the second string display
        const str2Div = document.createElement('div');
        str2Div.className = 'string-row';
        
        for (let i = 0; i < data.str2.length; i++) {
            const charDiv = document.createElement('div');
            charDiv.className = 'string-char';
            charDiv.textContent = data.str2.charAt(i);
            
            if (i === data.pos2) {
                charDiv.classList.add('highlighted');
            }
            
            str2Div.appendChild(charDiv);
        }
        
        // Create result display if present
        if (data.result) {
            const resultDiv = document.createElement('div');
            resultDiv.className = 'compare-result';
            resultDiv.textContent = data.result;
            compareDiv.appendChild(resultDiv);
        }
        
        compareDiv.appendChild(str1Div);
        compareDiv.appendChild(str2Div);
        container.appendChild(compareDiv);
    }
} 