/**
 * edulcni - Competitive Programming Visualization Library
 */
#pragma once

#include "edulcni/core/state.hpp"
#include "edulcni/core/frame.hpp"
#include "edulcni/core/widget.hpp"
#include "edulcni/core/export.hpp"

// Include widgets
#include "edulcni/widgets/array.hpp"
#include "edulcni/widgets/string_compare.hpp"

// Public API
namespace edulcni {
    // Core functions
    void initialize(const std::string& output_dir = "./viz_output");
    void finalize();
    void breakpoint();
    void freeze();
    void unfreeze();
    void export_visualization(const std::string& output_dir = "./viz_output");
} 