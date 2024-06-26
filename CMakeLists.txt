cmake_minimum_required(VERSION 3.10)

# Set the project name
project(TCPServer)

# Include directories
include_directories("${CMAKE_SOURCE_DIR}/lib")

# Glob source files for better organization
file(GLOB_RECURSE SERVER_SOURCES 
     "${CMAKE_SOURCE_DIR}/src/Server.cpp"
     "${CMAKE_SOURCE_DIR}/src/ClientHandler.cpp"
     "${CMAKE_SOURCE_DIR}/src/Client.cpp"
     "${CMAKE_SOURCE_DIR}/src/main.cpp")
file(GLOB_RECURSE CLIENT_SOURCES 
     "${CMAKE_SOURCE_DIR}/src/ClientFrontend.cpp"
     "${CMAKE_SOURCE_DIR}/src/ChatClient.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/lib/*.h")

# Add server executable
add_executable(server ${SERVER_SOURCES} ${HEADER_FILES})

# Add client executable
add_executable(chatClient ${CLIENT_SOURCES} ${HEADER_FILES})

# Specify C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find and link pthread library for the server executable
find_package(Threads REQUIRED)
target_link_libraries(server Threads::Threads)

# Find FLTK and link it
find_package(FLTK REQUIRED)
if(FLTK_FOUND)
    include_directories(${FLTK_INCLUDE_DIRS})
    target_link_libraries(chatClient ${FLTK_LIBRARIES})
else()
    message(FATAL_ERROR "FLTK not found")
endif()
