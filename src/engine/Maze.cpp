
#include "Maze.h"

using namespace std;

// Constructor
Maze::Maze(int r, int c) {

    rows = r;
    cols = c;

    rng.seed(random_device{}());

    grid.resize(rows, vector<int>(cols, 1));

    start = {1, 1};
    finish = {rows - 2, cols - 2};

    player = start;
}

// Check valid cell
bool Maze::isValid(int r, int c) {

    return (
        r >= 0 &&
        r < rows &&
        c >= 0 &&
        c < cols &&
        grid[r][c] == 0
    );
}

// Recursive DFS maze generation
void Maze::generateMazeDFS(int r, int c) {

    grid[r][c] = 0;

    vector<pair<int, int>> directions = {
        {-2, 0},
        {2, 0},
        {0, -2},
        {0, 2}
    };

    shuffle(directions.begin(), directions.end(), rng);

    for (auto direction : directions) {

        int nr = r + direction.first;
        int nc = c + direction.second;

        if (nr > 0 && nr < rows - 1 &&
            nc > 0 && nc < cols - 1 &&
            grid[nr][nc] == 1) {

            grid[r + direction.first / 2]
                [c + direction.second / 2] = 0;

            generateMazeDFS(nr, nc);
        }
    }
}

// Add loops by removing selected walls between open paths
void Maze::addLoops(double removalRatio) {

    vector<pair<int, int>> wallCandidates;

    for (int r = 1; r < rows - 1; r++) {
        for (int c = 1; c < cols - 1; c++) {
            if (grid[r][c] == 1) {
                bool verticalPassage = (grid[r - 1][c] == 0 && grid[r + 1][c] == 0);
                bool horizontalPassage = (grid[r][c - 1] == 0 && grid[r][c + 1] == 0);

                if (verticalPassage || horizontalPassage) {
                    wallCandidates.push_back({r, c});
                }
            }
        }
    }

    shuffle(wallCandidates.begin(), wallCandidates.end(), rng);

    int wallsToRemove = static_cast<int>(wallCandidates.size() * removalRatio);
    if (wallsToRemove < 3 && !wallCandidates.empty()) {
        wallsToRemove = std::min(static_cast<int>(wallCandidates.size()), 3);
    }

    for (int i = 0; i < wallsToRemove; i++) {
        grid[wallCandidates[i].first][wallCandidates[i].second] = 0;
    }
}

// Generate random maze
void Maze::generateMaze() {

    grid.assign(rows, vector<int>(cols, 1));

    generateMazeDFS(start.first, start.second);

    // Carve additional passages to create multiple paths & shortcuts
    addLoops(0.12);

    player = start;
}

// Display maze
void Maze::displayMaze() {

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            if (player.first == i && player.second == j) {
                cout << "P ";
            }
            else if (start.first == i && start.second == j) {
                cout << "S ";
            }
            else if (finish.first == i && finish.second == j) {
                cout << "E ";
            }
            else if (grid[i][j] == 1) {
                cout << "# ";
            }
            else {
                cout << ". ";
            }
        }

        cout << endl;
    }
}

// Move player
bool Maze::movePlayer(char direction) {

    int r = player.first;
    int c = player.second;

    if (direction == 'W' || direction == 'w') {
        r--;
    }
    else if (direction == 'S' || direction == 's') {
        r++;
    }
    else if (direction == 'A' || direction == 'a') {
        c--;
    }
    else if (direction == 'D' || direction == 'd') {
        c++;
    }
    else {
        return false;
    }

    if (isValid(r, c)) {

        player = {r, c};

        return true;
    }

    return false;
}

// Check completion
bool Maze::isCompleted() {

    return player == finish;
}

// Reset player
void Maze::resetPlayer() {

    player = start;
}

// Reconstruct path
vector<pair<int, int>> Maze::reconstructPath(
    vector<vector<pair<int, int>>>& parent
) {

    vector<pair<int, int>> path;

    pair<int, int> current = finish;

    while (current != start) {

        if (current.first == -1) {
            return {};
        }

        path.push_back(current);

        current = parent[current.first][current.second];
    }

    path.push_back(start);

    reverse(path.begin(), path.end());

    return path;
}

// Breadth First Search
vector<pair<int, int>> Maze::solveBFS() {

    vector<vector<bool>> visited(
        rows, vector<bool>(cols, false)
    );

    vector<vector<pair<int, int>>> parent(
        rows,
        vector<pair<int, int>>(cols, {-1, -1})
    );

    queue<pair<int, int>> q;

    q.push(start);

    visited[start.first][start.second] = true;

    vector<pair<int, int>> directions = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    while (!q.empty()) {

        pair<int, int> current = q.front();
        q.pop();

        if (current == finish) {
            return reconstructPath(parent);
        }

        for (auto direction : directions) {

            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (isValid(nr, nc) && !visited[nr][nc]) {

                visited[nr][nc] = true;

                parent[nr][nc] = current;

                q.push({nr, nc});
            }
        }
    }

    return {};
}

// Depth First Search
vector<pair<int, int>> Maze::solveDFS() {

    vector<vector<bool>> visited(
        rows, vector<bool>(cols, false)
    );

    vector<vector<pair<int, int>>> parent(
        rows,
        vector<pair<int, int>>(cols, {-1, -1})
    );

    stack<pair<int, int>> st;

    st.push(start);

    visited[start.first][start.second] = true;

    vector<pair<int, int>> directions = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    while (!st.empty()) {

        pair<int, int> current = st.top();
        st.pop();

        if (current == finish) {
            return reconstructPath(parent);
        }

        for (auto direction : directions) {

            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (isValid(nr, nc) && !visited[nr][nc]) {

                visited[nr][nc] = true;

                parent[nr][nc] = current;

                st.push({nr, nc});
            }
        }
    }

    return {};
}

// Return maze grid
const vector<vector<int>>& Maze::getGrid() const {

    return grid;
}

// Return start position
pair<int, int> Maze::getStart() const {

    return start;
}

// Return finish position
pair<int, int> Maze::getFinish() const {

    return finish;
}
