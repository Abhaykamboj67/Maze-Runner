
#include "../engine/Maze.h"
#include <limits>

using namespace std;

int main() {

    Maze maze(21, 21);

    maze.generateMaze();

    int choice;

    while (true) {

        cout << "\n========== MAZE RUNNER ==========\n";
        cout << "1. Display Maze\n";
        cout << "2. Play Manually\n";
        cout << "3. Solve using BFS\n";
        cout << "4. Solve using DFS\n";
        cout << "5. Generate New Maze\n";
        cout << "6. Exit\n";

        cout << "\nEnter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input!\n";
            continue;
        }

        if (choice == 1) {

            maze.displayMaze();

        }

        else if (choice == 2) {

            char move;

            while (!maze.isCompleted()) {

                maze.displayMaze();

                cout << "\nMove (W/A/S/D, Q to quit): ";
                cin >> move;

                if (move == 'Q' || move == 'q') {
                    break;
                }

                if (!maze.movePlayer(move)) {
                    cout << "Invalid move! Try again.\n";
                }
            }

            if (maze.isCompleted()) {
                cout << "\nCongratulations! You escaped the maze!\n";
            }

        }

        else if (choice == 3) {

            auto path = maze.solveBFS();

            if (path.empty()) {
                cout << "No path found!\n";
            }
            else {
                cout << "\nBFS Shortest Path Found!\n";
                cout << "Path length: " << path.size() - 1 << "\n";

                maze.displayMaze();

                cout << "\nPath coordinates:\n";

                for (auto cell : path) {
                    cout << "(" << cell.first
                         << ", " << cell.second << ") ";
                }

                cout << "\n";
            }

        }

        else if (choice == 4) {

            auto path = maze.solveDFS();

            if (path.empty()) {
                cout << "No path found!\n";
            }
            else {
                cout << "\nDFS Path Found!\n";
                cout << "Path length: " << path.size() - 1 << "\n";

                cout << "\nPath coordinates:\n";

                for (auto cell : path) {
                    cout << "(" << cell.first
                         << ", " << cell.second << ") ";
                }

                cout << "\n";
            }

        }

        else if (choice == 5) {

            maze.generateMaze();

            cout << "\nNew maze generated successfully!\n";

        }

        else if (choice == 6) {

            cout << "\nThanks for playing Maze Runner!\n";
            break;

        }

        else {

            cout << "\nInvalid choice! Try again.\n";

        }
    }

    return 0;
}
