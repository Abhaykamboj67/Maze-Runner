# 🏃 Maze Runner

An interactive Maze Runner application featuring both a **Console (Terminal) game** and a modern **Web Frontend with a custom C++ HTTP Server**.

It implements random maze generation with loops/shortcuts and solves mazes using **Breadth-First Search (BFS)** for finding the guaranteed shortest path and **Depth-First Search (DFS)** for deep exploration.

---

## 🌟 Features

- **Random Maze Generation**: Uses recursive DFS backtracking with loop braiding (~12% corridor bridge removal) to ensure **multiple paths, junctions, and shortcuts** to the exit.
- **Shortest Path AI**:
  - **BFS (Breadth-First Search)**: Guaranteed optimal shortest path.
  - **DFS (Depth-First Search)**: Exploratory path finding.
- **Dual Interface**:
  - **Web UI**: Modern dark-mode HTML5 Canvas interface with animated step-by-step solving, move counters, and keyboard navigation (WASD / Arrow Keys).
  - **Console UI**: Fast, pure terminal-based gameplay.
- **Custom C++ HTTP Server**: Native Winsock-based REST API serving static files (`HTML`, `CSS`, `JS`) and JSON endpoints without external framework dependencies.

---

## 📁 Project Structure

```text
MazeRunner/
├── CMakeLists.txt              # CMake build configuration
├── .gitignore                  # Git ignore rules (binaries, build artifacts)
├── README.md                   # Documentation
├── src/
│   ├── engine/
│   │   ├── Maze.h              # Maze class definition & algorithms
│   │   └── Maze.cpp            # Maze generation, BFS, DFS implementations
│   ├── console/
│   │   └── main_console.cpp    # CLI terminal game entry point
│   └── server/
│       ├── HttpServer.h        # HTTP Server header
│       ├── HttpServer.cpp      # Winsock HTTP server & REST API
│       └── main_server.cpp     # Server entry point
└── web/
    ├── index.html              # Frontend UI layout & controls
    ├── style.css               # Styling & responsive design
    └── script.js               # Canvas renderer, animations & API integration
```

---

## 🛠️ Build & Run Instructions

### Prerequisites
- GCC / MinGW (`g++` with C++17 support)
- Windows (for Winsock2 networking)

### Option 1: Web Server Application
1. Compile the server:
   ```powershell
   g++ -std=c++17 -Wall -o Server.exe src/engine/Maze.cpp src/server/HttpServer.cpp src/server/main_server.cpp -lws2_32
   ```
2. Run the server (or simply double-click `Server.exe`):
   ```powershell
   .\Server.exe
   ```
3. Open your browser at [http://localhost:3000](http://localhost:3000) (the server will also open it automatically!).

### Option 2: Console Terminal Game
1. Compile the console app:
   ```powershell
   g++ -std=c++17 -Wall -o MazeRunner.exe src/engine/Maze.cpp src/console/main_console.cpp
   ```
2. Run in your terminal:
   ```powershell
   .\MazeRunner.exe
   ```

---

## 🌐 API Endpoints

| Method | Endpoint | Description |
|---|---|---|
| `GET` | `/` | Serves web application |
| `GET` | `/api/maze` | Returns current maze grid, start, and finish points as JSON |
| `GET` | `/api/new` | Generates a new random maze and returns it |
| `GET` | `/api/solve?algorithm=bfs` | Returns BFS shortest path coordinates and length |
| `GET` | `/api/solve?algorithm=dfs` | Returns DFS path coordinates and length |
