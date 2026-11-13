
#ifndef MAZE_H
#define MAZE_H

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <utility>
#include <algorithm>
#include <random>

class Maze {

private:

    int rows;
    int cols;

    std::vector<std::vector<int>> grid;

    std::pair<int, int> start;
    std::pair<int, int> finish;
    std::pair<int, int> player;

    std::mt19937 rng;

    void generateMazeDFS(int r, int c);

    void addLoops(double removalRatio = 0.12);

    std::vector<std::pair<int, int>> reconstructPath(
        std::vector<std::vector<std::pair<int, int>>>& parent
    );

public:

    Maze(int r, int c);

    void generateMaze();

    void displayMaze();

    bool isValid(int r, int c);

    bool movePlayer(char direction);

    bool isCompleted();

    void resetPlayer();

    std::vector<std::pair<int, int>> solveBFS();

    std::vector<std::pair<int, int>> solveDFS();

    const std::vector<std::vector<int>>& getGrid() const;

    std::pair<int, int> getStart() const;

    std::pair<int, int> getFinish() const;

};

#endif
