
cmake_minimum_required(VERSION 3.10)
project(app C)

# Set C99 standard
add_compile_options(-std=c99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lraylib -lGL -lm -lpthread -ldl -lrt -lX11")
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


