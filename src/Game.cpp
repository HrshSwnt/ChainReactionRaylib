#include "main.hpp"

Game::Game() : rows(0), cols(0), playerCount(0), 
                currentPlayer(0), turns(0), pendingTurnChange(false), skipExplosions(false) {}
Game::~Game() = default;

Game& Game::instance() {
    static Game instance; // Singleton instance
    return instance;
}

void Game::initialize(int r, int c, int p, int cpu){
    rows = r;
    cols = c;
    playerCount = p + cpu;
    currentPlayer = 0;
    turns = 0;
    pendingTurnChange = false;
    skipExplosions = false;

    undoStack = std::stack<GameState>();
    redoStack = std::stack<GameState>();

    // reset the board
    Board.clear();

    Board.resize(rows, std::vector<Cell>(cols, Cell(0, 0)));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Board[i][j] = Cell(i, j);
        }
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (i > 0) Board[i][j].addNeighbor(&Board[i - 1][j]); // Up
            if (i < rows - 1) Board[i][j].addNeighbor(&Board[i + 1][j]); // Down
            if (j > 0) Board[i][j].addNeighbor(&Board[i][j - 1]); // Left
            if (j < cols - 1) Board[i][j].addNeighbor(&Board[i][j + 1]); // Right
        }
    }
    Players.clear();
    for (int i = 0; i < p; ++i) {
        Players.push_back(Player(i + 1, false)); // Player IDs start from 1
    }
    for (int i = 0; i < cpu; ++i) {
        Players.push_back(Player(p + i + 1, true)); // CPU players start from p + 1
    }
}

int Game::getPlayer() const {
    if (Players.empty() || currentPlayer < 0 || currentPlayer >= Players.size()) {
        return -1; // Invalid player ID
    }
    return Players[currentPlayer].getID(); // Return the current player's ID
}

void Game::changePlayer() {
    if (Players.size() < 2) return;  // Only one or zero players left

    currentPlayer++;
    if (currentPlayer >= Players.size()) {
        currentPlayer = 0;
    }
}


void Game::drawGame() const {

    int windowWidth = GetScreenWidth();
    int windowHeight = GetScreenHeight();
    float xOffset = rows * SPACING / 2;
    float yOffset = cols * SPACING / 2;
    float zOffset = depth / 2;

    // Camera looking down from above (top-down view)
    BeginMode3D(camera);

    // Draw grid lines (including borders)
    // Draw vertical lines (front and back)
    for (int i = 0; i <= rows; ++i) {
        float x = i * SPACING;
        // Front face (z = 0)
        DrawLine3D({x, 0, 0}, {x, static_cast<float>(cols) * SPACING, 0}, LIGHTGRAY);
        // Back face (z = depth)
        DrawLine3D({x, 0, depth}, {x, static_cast<float>(cols) * SPACING, depth}, LIGHTGRAY);
    }
    // Draw horizontal lines (front and back)
    for (int j = 0; j <= cols; ++j) {
        float y = j * SPACING;
        // Front face (z = 0)
        DrawLine3D({0, y, 0}, {static_cast<float>(rows) * SPACING, y, 0}, LIGHTGRAY);
        // Back face (z = depth)
        DrawLine3D({0, y, depth}, {static_cast<float>(rows) * SPACING, y, depth}, LIGHTGRAY);
    }
    // Draw vertical lines connecting front and back faces (borders)
    for (int i = 0; i <= rows; ++i) {
        float x = i * SPACING;
        DrawLine3D({x, 0, 0}, {x, 0, depth}, LIGHTGRAY); // Left border
        DrawLine3D({x, static_cast<float>(cols) * SPACING, 0}, {x, static_cast<float>(cols) * SPACING, depth}, LIGHTGRAY); // Right border
    }
    // Draw horizontal lines connecting front and back faces (borders)
    for (int j = 0; j <= cols; ++j) {
        float y = j * SPACING;
        DrawLine3D({0, y, 0}, {0, y, depth}, LIGHTGRAY); // Top border
        DrawLine3D({static_cast<float>(rows) * SPACING, y, 0}, {static_cast<float>(rows) * SPACING, y, depth}, LIGHTGRAY); // Bottom border
    }

    for (const auto& row : Board) {
        for (const auto& cell : row) {
            cell.drawCell();
        }
    }
    
    drawExplosions();
    EndMode3D();
    for (const auto& player : Players) {
        Color playerColor = PlayerIDtoColor(player.getID());
        std::string playerName = PlayerIDtoName(player.getID());
        if (player.isCPUPlayer()) {
            playerName += " (CPU)";
        }
        int count = 0;
        for (const auto& row : Board) {
            for (const auto& cell : row) {
                if (cell.p == player.getID()) {
                    count++;
                }
            }
        }
        std::string playerText = playerName + " (" + std::to_string(count) + ")";
        DrawText(playerText.c_str(), 10, 20 * player.getID(), 20, playerColor);
    }
    std::string cursor_info = "Cursor is on cell: (" + std::to_string(static_cast<int>(screenToGrid(GetMousePosition()).x)) + ", " + std::to_string(static_cast<int>(screenToGrid(GetMousePosition()).y)) + ")";
    int textWidth = MeasureText(cursor_info.c_str(), 20);
    DrawText(cursor_info.c_str(), windowWidth - textWidth - 10, windowHeight - 30, 20, WHITE);
}


void Game::press(float x, float y) {
    Vector2 gridPos = screenToGrid({x, y});
    int X = static_cast<int>(gridPos.x);
    int Y = static_cast<int>(gridPos.y);
    if (X < 0 || X >= rows || Y < 0 || Y >= cols) {
        return; // Out of bounds
    }
    if (X >= 0 && X < rows && Y >= 0 && Y < cols && currentPlayer >= 0 && currentPlayer < playerCount) {
        GameState currentState = getCurrentState();

        if (Board[X][Y].incr(getPlayer(), true)) {
            undoStack.push(currentState);
            if (redoStack.size() > 0) {
                redoStack = std::stack<GameState>(); // Clear redo stack if a new action is taken
            }
            turns++;
            if (explosionQueue.empty()) {
                changePlayer();
            } else {
                pendingTurnChange = true;
            }
        }
    }
}

void Game::CPUpress(int X, int Y) {
    if (X < 0 || X >= rows || Y < 0 || Y >= cols) {
        return; // Out of bounds
    }
    if (X >= 0 && X < rows && Y >= 0 && Y < cols && currentPlayer >= 0 && currentPlayer < playerCount) {
        GameState currentState = getCurrentState();

        if (Board[X][Y].incr(getPlayer(), false)) {
            undoStack.push(currentState);
            if (redoStack.size() > 0) {
                redoStack = std::stack<GameState>(); // Clear redo stack if a new action is taken
            }
            turns++;
            if (explosionQueue.empty()) {
                changePlayer();
            } else {
                pendingTurnChange = true;
            }
        }
    }
}

void Game::eliminatePlayer(int playerID) {
    int currentPlayerID = getPlayer();
    auto it = std::find_if(Players.begin(), Players.end(), [playerID](const Player& player) {
        return player.getID() == playerID;
    });
    if (it != Players.end()) {
        Players.erase(it); // Remove player ID from the list
    }
    // set currentPlayer to index of currentPlayerID in Players
    for (size_t i = 0; i < Players.size(); ++i) {
        if (Players[i].getID() == currentPlayerID) {
            currentPlayer = static_cast<int>(i);
            break;
        }
    }
}

int Game::intermediaryGameEndCheck() {
    if (Players.size() == 1) {
        return Players.front().getID(); // Return the winning player ID
    }
    if (turns > playerCount) {
        std::map<int, int> p_count; // Map to count players
        for (int i = 1; i <= playerCount; ++i) {
            p_count[i] = 0;
        }
        for (const auto& row : Board) {
            for (const auto& cell : row) {
                if (cell.p > 0 && cell.p <= playerCount) {
                    p_count[cell.p]++;
                }
            }
        }

        for (int i = playerCount; i >= 1; --i) {
            if (p_count[i] == 0) {
                eliminatePlayer(i); // Remove player ID from the list
            }
        }
        if (Players.size() == 1) {
            return Players.front().getID(); // Return the winning player ID
        }
    }
    return 0;
}

int Game::gameEndCheck() {
    if (Players.size() == 1) {
        return Players.front().getID(); // Return the winning player ID
    }
    if (turns > playerCount && explosionQueue.empty()) {
        std::map<int, int> p_count; // Map to count players
        for (int i = 1; i <= playerCount; ++i) {
            p_count[i] = 0;
        }
        for (const auto& row : Board) {
            for (const auto& cell : row) {
                if (cell.p > 0 && cell.p <= playerCount) {
                    p_count[cell.p]++;
                }
            }
        }

        for (int i = playerCount; i >= 1; --i) {
            if (p_count[i] == 0) {
                eliminatePlayer(i); // Remove player ID from the list
            }
        }
        if (Players.size() == 1) {
            return Players.front().getID(); // Return the winning player ID
        }
    }
    return 0;
}

Vector2 Game::screenToGrid(Vector2 screenPos) const {
    // Convert screen coordinates to grid coordinates
    Ray ray = GetMouseRay(screenPos, camera);

    // Plane at z = depth
    float t = (depth - ray.position.z) / ray.direction.z;
    float x = ray.position.x + ray.direction.x * t;
    float y = ray.position.y + ray.direction.y * t;

    // Convert world coordinates to grid indices
    int gridX = static_cast<int>(x / SPACING);
    int gridY = static_cast<int>(y / SPACING);
    return { (float)gridX, (float)gridY };
}




