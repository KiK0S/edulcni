cmake_minimum_required(VERSION 3.10)
project(edulcni VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add example executables
add_executable(array_example examples/array_example.cpp)
add_executable(string_bubble_sort examples/string_bubble_sort.cpp)
add_executable(std_sort_example examples/std_sort_example.cpp)
add_executable(mixed_types_example examples/mixed_types_example.cpp) 