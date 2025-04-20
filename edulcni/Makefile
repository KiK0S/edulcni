CXX = emcc
CXXFLAGS = -s WASM=1 \
    -s "EXPORTED_FUNCTIONS=['_main','_invoke_cell_callback','_invoke_button_callback']" \
    -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']" \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s USE_PTHREADS=0 \
    -s ASYNCIFY=1 \
    -s "ASYNCIFY_IMPORTS=['emscripten_sleep','emscripten_sleep_with_yield']" \
    -s DISABLE_EXCEPTION_CATCHING=0 \
    -s "EXPORT_ALL=1" \
    -s "ASSERTIONS=2" \
    -lembind \
    -O2 \
    --std=c++17 \
    -I include

SRC_DIR = src
EXAMPLE_DIR = examples
WEB_DIR = web

# Only include required implementation files
SRCS = $(SRC_DIR)/widgets/game_board.cpp \
       $(EXAMPLE_DIR)/tic_tac_toe.cpp

all: tic_tac_toe

tic_tac_toe: $(SRCS)
	$(CXX) -o $(WEB_DIR)/js/tic_tac_toe.js $^ $(CXXFLAGS)
	@echo "Build complete."
	@echo "To run the application:"
	@echo "1. cd $(WEB_DIR)"
	@echo "2. python -m http.server 8000"
	@echo "3. Open http://localhost:8000/tic_tac_toe.html in your browser"

clean:
	rm -f $(WEB_DIR)/js/tic_tac_toe.js $(WEB_DIR)/js/tic_tac_toe.wasm

.PHONY: all clean