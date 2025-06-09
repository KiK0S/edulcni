# Edulcni Framework Makefile
# Supports both static (g++) and interactive (emscripten) builds

# Build mode configuration
BUILD_MODE ?= static

# Static build configuration (default)
ifeq ($(BUILD_MODE),static)
    CXX = g++
    CXXFLAGS = -std=c++17 -O2 -g
    TARGET_SUFFIX = 
    OUTPUT_EXT = 
endif

# Interactive build configuration
ifeq ($(BUILD_MODE),interactive)
    CXX = em++
    CXXFLAGS = -std=c++17 -O1 -DEDULCNI_EMSCRIPTEN \
               -s WASM=1 \
               -s ALLOW_MEMORY_GROWTH=1 \
               -s EXPORTED_FUNCTIONS='["_main","_invoke_callback","_invoke_callback_with_data"]' \
               -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
               --bind \
               -s FORCE_FILESYSTEM=1 \
               -s DISABLE_EXCEPTION_CATCHING=0 \
               -s ASSERTIONS=1 \
               -s SAFE_HEAP=1 \
               -s STACK_OVERFLOW_CHECK=2 \
               -s DEMANGLE_SUPPORT=1 \
               -s ASYNCIFY=1 \
               -s ASYNCIFY_IMPORTS='["emscripten_sleep"]' \
               -g3 \
               --source-map-base http://localhost:8000/
    TARGET_SUFFIX = _interactive
    OUTPUT_EXT = .js
endif

# Shared library build configuration
ifeq ($(BUILD_MODE),shared)
    CXX = g++
    CXXFLAGS = -std=c++17 -O2 -g -fPIC
    TARGET_SUFFIX = 
    OUTPUT_EXT = .so
    SHARED_TARGET = lib/libedulcni.so
endif

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build
EXAMPLEDIR = examples
LIBDIR = lib

# Find all source files
CORE_SOURCES = $(shell find $(SRCDIR)/core -name "*.cpp" 2>/dev/null || echo "")
WIDGET_SOURCES = $(shell find $(SRCDIR)/widgets -name "*.cpp" 2>/dev/null || echo "")
SOURCES = $(CORE_SOURCES) $(WIDGET_SOURCES)

OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

# Target configuration
TARGET_BASE = edulcni
TARGET = $(TARGET_BASE)$(TARGET_SUFFIX)
OUTPUT_TARGET = $(TARGET)$(OUTPUT_EXT)

# Examples
ALL_STATIC_EXAMPLES = $(shell find $(EXAMPLEDIR) -name "*.cpp" -exec basename {} .cpp \; 2>/dev/null || echo "")
STATIC_EXAMPLES = $(filter-out tic_tac_toe,$(ALL_STATIC_EXAMPLES))
INTERACTIVE_EXAMPLES = tic_tac_toe
SHARED_EXAMPLES = tic_tac_toe

.PHONY: all clean examples help static interactive shared debug install
.PHONY: $(STATIC_EXAMPLES) $(INTERACTIVE_EXAMPLES)

# Default target
all: 
	@echo "Building in $(BUILD_MODE) mode..."
	@$(MAKE) $(OUTPUT_TARGET)

# Aliases for different build modes
static:
	@$(MAKE) BUILD_MODE=static all

interactive:
	@$(MAKE) BUILD_MODE=interactive 

shared:
	@$(MAKE) BUILD_MODE=shared $(SHARED_TARGET) 

# Build main library
$(OUTPUT_TARGET): $(OBJECTS)
	@echo "Linking $(OUTPUT_TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^
ifeq ($(BUILD_MODE),interactive)
	@echo "Build complete: $(TARGET).js and $(TARGET).wasm"
else
	@echo "Build complete: $(TARGET)"
endif

# Build shared library
$(SHARED_TARGET): $(OBJECTS)
	@echo "Building shared library $(SHARED_TARGET)..."
	@mkdir -p $(LIBDIR)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^
	@echo "Shared library created: $(SHARED_TARGET)"
	@echo "Headers available in: $(INCDIR)/"
	@echo ""
	@echo "To use in your project:"
	@echo "  g++ -std=c++17 -I$(PWD)/$(INCDIR) your_code.cpp -L$(PWD)/$(LIBDIR) -ledulcni -o your_program"

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Build examples
examples:
ifeq ($(BUILD_MODE),static)
	@echo "Building static examples..."
	@$(MAKE) $(STATIC_EXAMPLES) $(SHARED_EXAMPLES)
else
	@echo "Building interactive examples..."
	@$(MAKE) $(INTERACTIVE_EXAMPLES)
endif

# Static examples (generate individual targets)
$(STATIC_EXAMPLES): %: $(EXAMPLEDIR)/%.cpp $(SOURCES)
	@echo "Building static example: $@"
	@mkdir -p examples/static
	$(CXX) $(CXXFLAGS) -I$(INCDIR) $< $(SOURCES) -o examples/static/$@

# Special handling for tic_tac_toe (exists in both modes)
tic_tac_toe:
ifeq ($(BUILD_MODE),interactive)
	@echo "Building interactive tic-tac-toe example..."
	@mkdir -p examples/interactive
	$(CXX) $(CXXFLAGS) -I$(INCDIR) \
		$(EXAMPLEDIR)/tic_tac_toe.cpp \
		$(SOURCES) \
		-o examples/interactive/tic_tac_toe.js
	@echo "Generating HTML page for interactive example..."
	@$(MAKE) generate_interactive_html EXAMPLE_NAME=tic_tac_toe
else
	@echo "Building static tic-tac-toe example..."
	@mkdir -p examples/static
	$(CXX) $(CXXFLAGS) -I$(INCDIR) \
		$(EXAMPLEDIR)/tic_tac_toe.cpp \
		$(SOURCES) \
		-o examples/static/tic_tac_toe
endif

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILDDIR)
	rm -f $(TARGET_BASE) $(TARGET_BASE)_interactive $(TARGET_BASE).js $(TARGET_BASE)_interactive.js $(TARGET_BASE)*.wasm
	rm -rf examples/static examples/interactive examples/shared
	rm -rf $(LIBDIR)

# Development build with debug info
debug: 
ifeq ($(BUILD_MODE),interactive)
	@$(MAKE) BUILD_MODE=interactive CXXFLAGS="$(CXXFLAGS) -g4 -O0 -s ASSERTIONS=2 -s RUNTIME_DEBUG=1" $(OUTPUT_TARGET)
else
	@$(MAKE) BUILD_MODE=static CXXFLAGS="$(CXXFLAGS) -g -O0" $(OUTPUT_TARGET)
endif

# Debug build for tic_tac_toe specifically
debug_tic_tac_toe:
	@$(MAKE) BUILD_MODE=interactive debug
	@$(MAKE) BUILD_MODE=interactive tic_tac_toe

# Build shared library demo
shared_demo: shared
	@echo "Building shared library demonstration..."
	@mkdir -p examples/shared
	$(CXX) -std=c++17 -I$(INCDIR) $(EXAMPLEDIR)/shared_library_demo.cpp -L$(LIBDIR) -ledulcni -o examples/shared/shared_demo
	@echo "Shared library demo built: examples/shared/shared_demo"
	@echo ""
	@echo "Run with: ./examples/shared/shared_demo"
	@echo "Make sure LD_LIBRARY_PATH includes: $(PWD)/$(LIBDIR)"

# Install target for shared library
install: shared
	@echo "Installing edulcni shared library..."
	@echo "Note: This is a local install. For system-wide install, run with sudo and modify paths."
	@mkdir -p $(HOME)/.local/lib $(HOME)/.local/include
	cp $(SHARED_TARGET) $(HOME)/.local/lib/
	cp -r $(INCDIR)/* $(HOME)/.local/include/
	@echo "Installed to:"
	@echo "  Library: $(HOME)/.local/lib/libedulcni.so"
	@echo "  Headers: $(HOME)/.local/include/"
	@echo ""
	@echo "Add to your .bashrc or .zshrc:"
	@echo "  export LD_LIBRARY_PATH=\"$(HOME)/.local/lib:\$$LD_LIBRARY_PATH\""
	@echo ""
	@echo "Compile with:"
	@echo "  g++ -std=c++17 -I$(HOME)/.local/include your_code.cpp -L$(HOME)/.local/lib -ledulcni -o your_program"

# Help target
help:
	@echo "Edulcni Framework Build System"
	@echo ""
	@echo "Build Modes:"
	@echo "  static      - Build with g++ for static HTML export (default)"
	@echo "  interactive - Build with emscripten for browser interaction"
	@echo "  shared      - Build as shared library for linking with C++ projects"
	@echo ""
	@echo "Targets:"
	@echo "  all         - Build the main library"
	@echo "  examples    - Build example applications"
	@echo "  shared      - Build shared library"
	@echo "  install     - Build and install shared library locally"
	@echo "  debug       - Build with debug information"
	@echo "  clean       - Remove all build files"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make                          # Build static library"
	@echo "  make static                   # Build static library (explicit)"
	@echo "  make interactive              # Build interactive library"
	@echo "  make shared                   # Build shared library"
	@echo "  make install                  # Build and install shared library"
	@echo "  make shared_demo              # Build shared library demonstration"
	@echo "  make examples                 # Build examples for current mode"
	@echo "  make BUILD_MODE=static examples       # Build static examples"
	@echo "  make BUILD_MODE=interactive examples  # Build interactive examples"
	@echo "  make tic_tac_toe              # Build tic-tac-toe for current mode"
	@echo "  make debug                    # Debug build for current mode"
	@echo "  make BUILD_MODE=interactive debug     # Interactive debug build"
	@echo ""
	@echo "Available Examples:"
	@echo "  Static only: $(STATIC_EXAMPLES)"
	@echo "  Interactive only: $(filter-out $(SHARED_EXAMPLES),$(INTERACTIVE_EXAMPLES))"
	@echo "  Both modes: $(SHARED_EXAMPLES)"
	@echo ""
	@echo "Requirements:"
	@echo "  Static mode:      g++ compiler"
	@echo "  Interactive mode: Emscripten SDK installed and activated"
	@echo "  Shared library:   g++ compiler"

# Generate HTML for interactive examples
generate_interactive_html:
	@echo "Generating index.html for $(EXAMPLE_NAME)..."
	@if [ ! -f static/templates/emscripten_viewer_template.html ]; then \
		echo "Error: Emscripten template not found at static/templates/emscripten_viewer_template.html"; \
		exit 1; \
	fi
	@sed -e 's/$$TITLE/$(EXAMPLE_NAME)/g' -e 's/$$JS_FILE_PATH/$(EXAMPLE_NAME).js/g' \
		static/templates/emscripten_viewer_template.html > examples/interactive/$(EXAMPLE_NAME).html
	@echo "Generated: examples/interactive/$(EXAMPLE_NAME).html"
	@echo "Open examples/interactive/$(EXAMPLE_NAME).html in your browser to run the interactive example"
	@echo ""
	@echo "Files created:"
	@echo "  - examples/interactive/$(EXAMPLE_NAME).js   (WebAssembly application)"
	@echo "  - examples/interactive/$(EXAMPLE_NAME).wasm (WebAssembly binary)"  
	@echo "  - examples/interactive/$(EXAMPLE_NAME).html (HTML viewer)"

# Create necessary directories
$(shell mkdir -p $(BUILDDIR)/core $(BUILDDIR)/widgets examples $(LIBDIR))