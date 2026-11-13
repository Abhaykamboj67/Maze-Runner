
#include "../engine/Maze.h"
#include "HttpServer.h"

int main() {

    Maze maze(21, 21);

    maze.generateMaze();

    startServer(maze, 3000);

    return 0;
}
