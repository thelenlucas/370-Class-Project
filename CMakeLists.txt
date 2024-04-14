cmake_minimum_required(VERSION 3.10)

# Set the project name
project(TCPServer)

# Include directories
include_directories("${CMAKE_SOURCE_DIR}/lib")

# Glob source files for better organization
file(GLOB_RECURSE SERVER_SOURCES "${CMAKE_SOURCE_DIR}/src/Server.cpp"
                                 "${CMAKE_SOURCE_DIR}/src/ClientHandler.cpp"
                                 "${CMAKE_SOURCE_DIR}/src/Client.cpp"
                                 "${CMAKE_SOURCE_DIR}/src/main.cpp") # Make sure main.cpp is included here
file(GLOB_RECURSE CLIENT_SOURCES "${CMAKE_SOURCE_DIR}/src/test_client.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/lib/*.h")

# Add server executable
add_executable(server ${SERVER_SOURCES} ${HEADER_FILES})

# Add client executable
add_executable(test_client ${CLIENT_SOURCES} ${HEADER_FILES})

# Specify C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find and link pthread library for the server executable
find_package(Threads REQUIRED)
target_link_libraries(server Threads::Threads)

# Find FLTK and link it
find_package(FLTK REQUIRED)
include_directories(${FLTK_INCLUDE_DIR})
target_link_libraries(test_client ${FLTK_LIBRARIES})