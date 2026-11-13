cmake_minimum_required(VERSION 3.10)
project(MazeRunner)

set(CMAKE_CXX_STANDARD 17)

# Console application
add_executable(MazeRunner
    src/engine/Maze.cpp
    src/console/main_console.cpp
)

# Server application
add_executable(MazeServer
    src/engine/Maze.cpp
    src/server/HttpServer.cpp
    src/server/main_server.cpp
)

target_link_libraries(MazeServer ws2_32)
