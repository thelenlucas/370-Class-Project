cmake_minimum_required(VERSION 3.10)

# Set the project name
project(TCPServer)

# Glob header files from the /lib directory
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/lib/*.h")

# Include directories
include_directories("${CMAKE_SOURCE_DIR}/lib")

# Add executable
add_executable(TCPServer main.cpp ${HEADER_FILES})

# Specify C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)