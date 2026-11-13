// ===== Configuration =====
const API_BASE = "";
const CELL_SIZE = 26;
const WALL_COLOR = "#2a2a3e";
const PATH_COLOR = "#1a1a2e";
const START_COLOR = "#4CAF50";
const FINISH_COLOR = "#f44336";
const PLAYER_COLOR = "#2196F3";
const SOLUTION_COLOR = "#FF9800";
const SOLUTION_TRAIL = "rgba(255, 152, 0, 0.25)";

// ===== State =====
let mazeData = null;
let playerPos = null;
let playerTrail = [];
let moveCount = 0;
let solutionPath = [];
let animating = false;
let won = false;

// ===== DOM Elements =====
const canvas = document.getElementById("maze-canvas");
const ctx = canvas.getContext("2d");
const statusEl = document.getElementById("status");
const moveCountEl = document.getElementById("move-count");
const btnNew = document.getElementById("btn-new");
const btnBFS = document.getElementById("btn-bfs");
const btnDFS = document.getElementById("btn-dfs");
const btnReset = document.getElementById("btn-reset");

// ===== API Calls =====
async function fetchMaze() {
    try {
        const res = await fetch(`${API_BASE}/api/maze`);
        if (!res.ok) throw new Error("Server error");
        return await res.json();
    } catch (err) {
        setStatus("❌ Cannot connect to server. Start MazeServer.exe first!", "error");
        return null;
    }
}

async function fetchNewMaze() {
    try {
        const res = await fetch(`${API_BASE}/api/new`);
        if (!res.ok) throw new Error("Server error");
        return await res.json();
    } catch (err) {
        setStatus("❌ Cannot connect to server.", "error");
        return null;
    }
}

async function fetchSolution(algorithm) {
    try {
        const res = await fetch(`${API_BASE}/api/solve?algorithm=${algorithm}`);
        if (!res.ok) throw new Error("Server error");
        return await res.json();
    } catch (err) {
        setStatus("❌ Cannot connect to server.", "error");
        return null;
    }
}

// ===== Status =====
function setStatus(msg, type = "") {
    statusEl.textContent = msg;
    statusEl.className = "status-msg" + (type ? ` ${type}` : "");
}

function updateMoveCount() {
    moveCountEl.textContent = `Moves: ${moveCount}`;
}

// ===== Drawing =====
function drawMaze() {
    if (!mazeData) return;

    const grid = mazeData.grid;
    const rows = grid.length;
    const cols = grid[0].length;

    canvas.width = cols * CELL_SIZE;
    canvas.height = rows * CELL_SIZE;

    // Clear
    ctx.fillStyle = "#0f0f1a";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Draw grid cells
    for (let r = 0; r < rows; r++) {
        for (let c = 0; c < cols; c++) {
            const x = c * CELL_SIZE;
            const y = r * CELL_SIZE;

            if (grid[r][c] === 1) {
                // Wall
                ctx.fillStyle = WALL_COLOR;
                ctx.fillRect(x, y, CELL_SIZE, CELL_SIZE);

                // Subtle border for depth
                ctx.fillStyle = "rgba(255,255,255,0.03)";
                ctx.fillRect(x, y, CELL_SIZE, 1);
                ctx.fillRect(x, y, 1, CELL_SIZE);
            } else {
                // Path
                ctx.fillStyle = PATH_COLOR;
                ctx.fillRect(x, y, CELL_SIZE, CELL_SIZE);
            }
        }
    }

    // Draw player footprint trail
    if (playerTrail.length > 1) {
        // Subtle line connecting player path
        ctx.strokeStyle = "rgba(33, 150, 243, 0.45)";
        ctx.lineWidth = 4;
        ctx.lineCap = "round";
        ctx.lineJoin = "round";
        ctx.beginPath();
        for (let i = 0; i < playerTrail.length; i++) {
            const [tr, tc] = playerTrail[i];
            const tx = tc * CELL_SIZE + CELL_SIZE / 2;
            const ty = tr * CELL_SIZE + CELL_SIZE / 2;
            if (i === 0) ctx.moveTo(tx, ty);
            else ctx.lineTo(tx, ty);
        }
        ctx.stroke();

        // Glowing breadcrumb dots
        for (let i = 0; i < playerTrail.length; i++) {
            const [tr, tc] = playerTrail[i];
            const tx = tc * CELL_SIZE + CELL_SIZE / 2;
            const ty = tr * CELL_SIZE + CELL_SIZE / 2;
            ctx.fillStyle = "rgba(100, 181, 246, 0.8)";
            ctx.beginPath();
            ctx.arc(tx, ty, 2.5, 0, Math.PI * 2);
            ctx.fill();
        }
    }

    // Draw solution trail
    if (solutionPath.length > 0) {
        for (const [r, c] of solutionPath) {
            const x = c * CELL_SIZE;
            const y = r * CELL_SIZE;
            ctx.fillStyle = SOLUTION_TRAIL;
            ctx.fillRect(x, y, CELL_SIZE, CELL_SIZE);

            // Dot in center
            ctx.fillStyle = SOLUTION_COLOR;
            ctx.beginPath();
            ctx.arc(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 3, 0, Math.PI * 2);
            ctx.fill();
        }
    }

    // Draw start
    const [sr, sc] = mazeData.start;
    drawSpecialCell(sc * CELL_SIZE, sr * CELL_SIZE, START_COLOR, "S");

    // Draw finish
    const [fr, fc] = mazeData.finish;
    drawSpecialCell(fc * CELL_SIZE, fr * CELL_SIZE, FINISH_COLOR, "🏁");

    // Draw player
    if (playerPos) {
        const [pr, pc] = playerPos;
        const px = pc * CELL_SIZE;
        const py = pr * CELL_SIZE;

        // Glow
        ctx.shadowColor = PLAYER_COLOR;
        ctx.shadowBlur = 12;

        ctx.fillStyle = PLAYER_COLOR;
        ctx.beginPath();
        ctx.arc(px + CELL_SIZE / 2, py + CELL_SIZE / 2, CELL_SIZE / 2 - 3, 0, Math.PI * 2);
        ctx.fill();

        ctx.shadowBlur = 0;

        // "P" label
        ctx.fillStyle = "#fff";
        ctx.font = `bold ${CELL_SIZE * 0.5}px sans-serif`;
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";
        ctx.fillText("P", px + CELL_SIZE / 2, py + CELL_SIZE / 2);
    }
}

function drawSpecialCell(x, y, color, label) {
    // Background
    ctx.fillStyle = color;
    ctx.beginPath();
    ctx.roundRect(x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4, 4);
    ctx.fill();

    // Label
    ctx.fillStyle = "#fff";
    ctx.font = `bold ${CELL_SIZE * 0.45}px sans-serif`;
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.fillText(label, x + CELL_SIZE / 2, y + CELL_SIZE / 2);
}

// ===== Player Movement =====
function movePlayer(dr, dc) {
    if (!mazeData || animating || won) return;

    const [pr, pc] = playerPos;
    const nr = pr + dr;
    const nc = pc + dc;

    // Bounds check
    if (nr < 0 || nr >= mazeData.grid.length || nc < 0 || nc >= mazeData.grid[0].length) {
        return;
    }

    // Wall check
    if (mazeData.grid[nr][nc] === 1) {
        return;
    }

    playerPos = [nr, nc];
    playerTrail.push([nr, nc]);
    moveCount++;
    updateMoveCount();
    solutionPath = [];
    drawMaze();

    // Win check
    const [fr, fc] = mazeData.finish;
    if (nr === fr && nc === fc) {
        won = true;
        setStatus("🎉 Congratulations! You escaped the maze!", "win");
    }
}

// ===== Solution Animation =====
async function animateSolution(path, algorithmName = "") {
    if (!path || path.length === 0) {
        setStatus("No solution path found!", "error");
        return;
    }

    animating = true;
    setButtons(false);
    solutionPath = [];
    playerTrail = [];

    for (let i = 0; i < path.length; i++) {
        solutionPath.push(path[i]);
        playerPos = path[i];
        moveCount = i;
        updateMoveCount();
        drawMaze();
        await sleep(35);
    }

    const algoLabel = algorithmName ? ` via ${algorithmName.toUpperCase()}` : "";
    const note = algorithmName.toLowerCase() === "bfs" ? " ⚡ Guaranteed Shortest Path!" : " (Exploration Path)";
    setStatus(`✅ Solved${algoLabel}! Path length: ${path.length - 1} steps${note}`);
    animating = false;
    setButtons(true);
}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function setButtons(enabled) {
    btnNew.disabled = !enabled;
    btnBFS.disabled = !enabled;
    btnDFS.disabled = !enabled;
    btnReset.disabled = !enabled;
}

// ===== Event Handlers =====
btnNew.addEventListener("click", async () => {
    setButtons(false);
    setStatus("Generating new maze with multiple paths...");
    const data = await fetchNewMaze();
    if (data) {
        mazeData = data;
        playerPos = [...data.start];
        playerTrail = [[...data.start]];
        moveCount = 0;
        solutionPath = [];
        won = false;
        updateMoveCount();
        drawMaze();
        setStatus("New maze generated! Use W/A/S/D or Arrow Keys to move.");
    }
    setButtons(true);
});

btnBFS.addEventListener("click", async () => {
    if (!mazeData) return;
    setStatus("Solving with BFS (finding shortest path)...");
    const data = await fetchSolution("bfs");
    if (data && data.path) {
        await animateSolution(data.path, "BFS");
    }
});

btnDFS.addEventListener("click", async () => {
    if (!mazeData) return;
    setStatus("Solving with DFS (exploring paths)...");
    const data = await fetchSolution("dfs");
    if (data && data.path) {
        await animateSolution(data.path, "DFS");
    }
});

btnReset.addEventListener("click", () => {
    if (!mazeData || animating) return;
    playerPos = [...mazeData.start];
    playerTrail = [[...mazeData.start]];
    moveCount = 0;
    solutionPath = [];
    won = false;
    updateMoveCount();
    drawMaze();
    setStatus("Player reset. Use W/A/S/D or Arrow Keys to move.");
});

// Keyboard controls
document.addEventListener("keydown", (e) => {
    switch (e.key) {
        case "w": case "W": case "ArrowUp":
            e.preventDefault();
            movePlayer(-1, 0);
            break;
        case "s": case "S": case "ArrowDown":
            e.preventDefault();
            movePlayer(1, 0);
            break;
        case "a": case "A": case "ArrowLeft":
            e.preventDefault();
            movePlayer(0, -1);
            break;
        case "d": case "D": case "ArrowRight":
            e.preventDefault();
            movePlayer(0, 1);
            break;
    }
});

// ===== Initialize =====
async function init() {
    setStatus("Connecting to server...");
    const data = await fetchMaze();
    if (data) {
        mazeData = data;
        playerPos = [...data.start];
        playerTrail = [[...data.start]];
        moveCount = 0;
        won = false;
        updateMoveCount();
        drawMaze();
        setStatus("Use W/A/S/D or Arrow Keys to move. Reach the 🏁 to win!");
    }
}

init();
