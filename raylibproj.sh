#!/bin/bash


mkdir build
touch CMakeLists.txt
touch run.sh
mkdir sources
mkdir headers

echo "
cmake_minimum_required(VERSION 3.10)
project(app C)

# Set C99 standard
add_compile_options(-std=c99)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/headers)

# Source files
file(GLOB MAIN sources/*.c headers/*.h)

# Find required libraries (e.g., raylib)
find_package(raylib REQUIRED)

# Create the executable
add_executable(app ${MAIN})

# Link libraries (e.g., raylib)
target_link_libraries(app PRIVATE raylib)
" > CMakeLists.txt

echo "
#!/bin/bash

cd build

cmake ..
make

./app $@

" > run.sh
