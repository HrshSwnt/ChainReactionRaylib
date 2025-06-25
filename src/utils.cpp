#include "main.hpp"

std::vector<Color> colors = {
    RED, GREEN, BLUE, YELLOW, PURPLE, ORANGE, PINK, BROWN, GRAY
};

std::vector<std::string> playerNames = {
    "RED", "GREEN", "BLUE", "YELLOW", "PURPLE", "ORANGE", "PINK", "BROWN", "GRAY"
};


Camera3D camera;

void incrementFrameCount() {
    frameCount++;
}

int getFrameCount() {
    return frameCount;
}

Color PlayerIDtoColor(int playerID) {
    if (playerID < 1 || playerID >= colors.size()) {
        return BLACK; // Default color if out of range
    }
    return colors[playerID - 1]; // Player IDs start from 1, so we subtract 1 to get the index
}

std::string PlayerIDtoName(int playerID) {
    if (playerID < 1 || playerID >= playerNames.size()) {
        return "Unknown"; // Default name if out of range
    }
    return playerNames[playerID - 1]; // Player IDs start from 1, so we subtract 1 to get the index
}

float shakeStengthFromLevel(int level, int neighborCount) {
    return MIN_SHAKE_STRENGTH + (static_cast<float>(level) / static_cast<float>(neighborCount - 1)) * (MAX_SHAKE_STRENGTH - MIN_SHAKE_STRENGTH);   
}

void drawStart() {
    DrawText("Click to start", GetScreenWidth()/2, GetScreenHeight()/2, 20, WHITE);
}

void drawMenu() {
    DrawRectangleRec(menuRect, Fade(BLACK, 0.8f));
    DrawRectangleLinesEx(menuRect, 2, WHITE); // Draw border
    DrawText("Menu", menuRect.x + menuRect.width / 2.0f - MeasureText("Menu", 32) / 2, menuRect.y + 30, 32, WHITE);
    DrawText("Welcome to the Game!", menuRect.x + menuRect.width / 2.0f - MeasureText("Welcome to the Game!", 20) / 2, menuRect.y + 30 + 32 + 20, 20, WHITE);
    GuiSliderBar(rowSliderRect, "Rows ", std::to_string(static_cast<int>(rowValue)).c_str(), &rowValue, MIN_ROWS, MAX_ROWS);
    GuiSliderBar(colSliderRect, "Columns ", std::to_string(static_cast<int>(colValue)).c_str(), &colValue, MIN_COLS, MAX_COLS);
    GuiSliderBar(playerSliderRect, "Players ", std::to_string(static_cast<int>(playerValue)).c_str(), &playerValue, MIN_PLAYERS, MAX_PLAYERS);
    GuiButton( buttonRect, "Start Game");
}

void drawPlaying() {
    const char* gameTimeText = "Game Time";
    int gameTimeWidth = MeasureText(gameTimeText, 20);
    DrawText(gameTimeText, (GetScreenWidth() - gameTimeWidth) / 2, 0, 20, WHITE);

    std::string playerName = PlayerIDtoName(Game::instance().getPlayer());
    std::string currentPlayerText = "Current Player: " + playerName;
    int currentPlayerWidth = MeasureText(currentPlayerText.c_str(), 20);
    DrawText(currentPlayerText.c_str(), (GetScreenWidth() - currentPlayerWidth) / 2, 30, 20, WHITE);
    GuiButton(undoButtonRect, "Undo");
    GuiButton(redoButtonRect, "Redo");
    // show redo and undo stack size
    int textPadding = 10;
    int undoTextWidth = MeasureText(("Undo Stack Size: " + std::to_string(Game::instance().undoStack.size())).c_str(), 20);
    int redoTextWidth = MeasureText(("Redo Stack Size: " + std::to_string(Game::instance().redoStack.size())).c_str(), 20);

    DrawText(("Undo Stack Size: " + std::to_string(Game::instance().undoStack.size())).c_str(),
             GetScreenWidth() - undoTextWidth - textPadding,
             textPadding,
             20, WHITE);

    DrawText(("Redo Stack Size: " + std::to_string(Game::instance().redoStack.size())).c_str(),
             GetScreenWidth() - redoTextWidth - textPadding,
             textPadding + 30,
             20, WHITE);
    Game::instance().drawGame();
}

void drawGameOver() {
    drawPlaying();
    int winner = Game::instance().getPlayer();
    DrawText(("Game Over! Player " + PlayerIDtoName(winner) + " wins!").c_str(), GetScreenWidth()/2, GetScreenHeight()-50, 20, WHITE);

    GuiButton(restartButtonRect, "Restart Game");
}

void drawExit() {
    DrawText("Exiting game...", GetScreenWidth()/2, GetScreenHeight()/2, 20, WHITE);
}

void drawValidCursor() {
    Vector2 mousePos = GetMousePosition();
    Vector2 gridPos = Game::instance().screenToGrid(mousePos);
    int X = static_cast<int>(gridPos.x);
    int Y = static_cast<int>(gridPos.y);
    
    if (X >= 0 && X < Game::instance().rows && Y >= 0 && Y < Game::instance().cols) {
        Cell& cell = Game::instance().Board[X][Y];
        if (cell.p == 0 || cell.p == Game::instance().getPlayer()) { // Only draw cursor if the cell is empty
            Vector2 screenPos = GetMousePosition();
            DrawCircleV(screenPos, 10, GREEN);
            return;
        }
    }
    // Draw a red circle if the cursor is out of bounds
    Vector2 screenPos = GetMousePosition();
    DrawCircleV(screenPos, 10, RED);
}

void drawExplosions() {
    bool end = false;
    if (Game::instance().skipExplosions) {
        int maxStepsPerFrame = 100;
        int steps = 0;
        while (!explosionQueue.empty() && steps < maxStepsPerFrame) {
            PendingExplosion& explosion = explosionQueue.front();
            explosion.complete();
            explosionQueue.pop();
            steps++;
        }
        if (explosionQueue.empty()) {
            Game::instance().skipExplosions = false;
            end = true;
        }
        if (Game::instance().intermediaryGameEndCheck() > 0) {
            gameState = GAME_STATE_GAME_OVER;
        }
    } 
    else {
        int queueSize = explosionQueue.size();
        for (int i = 0; i < queueSize; ++i) {
            PendingExplosion& explosion = explosionQueue.front();
            explosion.update();
            if (explosion.isComplete()) {
                explosion.complete();
                explosionQueue.pop();
                if (explosionQueue.empty()) {
                    end = true;
                }
            } else {
                // Move incomplete explosion to the back of the queue
                explosionQueue.push(explosion);
                explosionQueue.pop();
            }
        }
        if (Game::instance().intermediaryGameEndCheck() > 0) {
            gameState = GAME_STATE_GAME_OVER;
        }
    }
    if (end) {
        if (Game::instance().pendingTurnChange) {
            Game::instance().changePlayer();
            Game::instance().pendingTurnChange = false;
        }
        if (Game::instance().gameEndCheck() > 0) {
            gameState = GAME_STATE_GAME_OVER;
        }


    }
}

void initializeCamera(int rows, int cols) {
    float xOffset = rows * SPACING / 2;
    float yOffset = cols * SPACING / 2;
    float zOffset = depth / 2;

    // Camera looking down from above (top-down view)
    camera = {
        {xOffset, yOffset, std::max(static_cast<float>(rows), static_cast<float>(cols)) * SPACING}, // Position in front of the center
        {xOffset, yOffset, zOffset}, // Target is the center of the grid
        {0, 1, 0}, // Up vector
        80.0f // FOV
    };
}

void resetCameraviaMiddleClick() {
    // Reset camera to the initial position
    float xOffset = Game::instance().rows * SPACING / 2;
    float yOffset = Game::instance().cols * SPACING / 2;
    float zOffset = depth / 2;

    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
        camera.position = {xOffset, yOffset, std::max(static_cast<float>(Game::instance().rows), static_cast<float>(Game::instance().cols)) * SPACING};
        camera.target = {xOffset, yOffset, zOffset};
    }
}

void moveCameraviaRightClick() {
    static Vector2 lastMousePos = {0, 0};
    static bool dragging = false;

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 mousePos = GetMousePosition();
        if (!dragging) {
            lastMousePos = mousePos;
            dragging = true;
        } else {
            float dx = mousePos.x - lastMousePos.x;
            float dy = mousePos.y - lastMousePos.y;

            float sensitivity = 0.005f;

            // Get vector from target to camera
            Vector3 offset = Vector3Subtract(camera.position, camera.target);
            float radius = Vector3Length(offset);

            // Convert offset to spherical coordinates
            float theta = atan2f(offset.x, offset.z); // azimuth angle (Y-axis)
            float phi = acosf(offset.y / radius);     // elevation angle (X-axis)

            // Apply mouse movement
            theta -= dx * sensitivity; // yaw (horizontal)
            phi -= dy * sensitivity;   // pitch (vertical)

            // Clamp phi to avoid flipping
            float epsilon = 0.01f;
            if (phi < epsilon) phi = epsilon;
            if (phi > PI - epsilon) phi = PI - epsilon;

            // Convert back to Cartesian
            offset.x = radius * sinf(phi) * sinf(theta);
            offset.y = radius * cosf(phi);
            offset.z = radius * sinf(phi) * cosf(theta);

            // Update camera position (target stays same)
            camera.position = Vector3Add(camera.target, offset);

            lastMousePos = mousePos;
        }
    } else {
        dragging = false;
    }
}

void zoomCameraViaScroll() {
    float scroll = GetMouseWheelMove();  // Positive = scroll up, Negative = scroll down
    if (scroll != 0.0f) {
        float zoomSpeed = 6.0f;  // You can tweak this value for desired sensitivity

        // Get direction vector from target to camera (zoom out direction)
        Vector3 dir = Vector3Subtract(camera.position, camera.target);
        dir = Vector3Normalize(dir);

        // Move camera along this direction
        Vector3 zoomOffset = Vector3Scale(dir, -scroll * zoomSpeed);
        camera.position = Vector3Add(camera.position, zoomOffset);
    }
}


void mousePressed(){
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        switch (gameState) {
            case GAME_STATE_START:
                gameState = GAME_STATE_MENU;
                break;
            case GAME_STATE_MENU:
                if (CheckCollisionPointRec(mousePos, buttonRect)) {
                    gameState = GAME_STATE_PLAYING;
                    int rows = static_cast<int>(rowValue);
                    int cols = static_cast<int>(colValue);
                    int players = static_cast<int>(playerValue);
                    initializeCamera(rows, cols); // Initialize camera based on user input
                    explosionQueue = std::queue<PendingExplosion>(); // Reset explosion queue
                    Game::instance().initialize(rows, cols, players); // Example initialization
                }
                break;
            case GAME_STATE_PLAYING:
                if (CheckCollisionPointRec(mousePos, undoButtonRect)) {
                    if (!Game::instance().undoStack.empty()) {
                        GameState state = Game::instance().undoStack.top();
                        explosionQueue = std::queue<PendingExplosion>(); // Reset explosion queue
                        Game::instance().redoStack.push(Game::instance().getCurrentState()); // Push current state to redo stacks
                        Game::instance().restoreFromState(state);
                        Game::instance().undoStack.pop();
                    }
                } else if (CheckCollisionPointRec(mousePos, redoButtonRect)) {
                    if (!Game::instance().redoStack.empty()) {
                        GameState state = Game::instance().redoStack.top();
                        explosionQueue = std::queue<PendingExplosion>(); // Reset explosion queue
                        Game::instance().undoStack.push(Game::instance().getCurrentState()); // Push current state to undo stack
                        Game::instance().restoreFromState(state);
                        Game::instance().redoStack.pop();
                    }
                } else if (explosionQueue.empty()) {
                    Game::instance().press(mousePos.x, mousePos.y);
                } else {
                    Game::instance().skipExplosions = true; // Skip explosions if any are pending
                }
                break;
            case GAME_STATE_GAME_OVER:
                if (CheckCollisionPointRec(mousePos, restartButtonRect)) {
                    gameState = GAME_STATE_MENU; // Restart the game
                } else if (CheckCollisionPointRec(mousePos, undoButtonRect)) {
                    if (!Game::instance().undoStack.empty()) {
                        GameState state = Game::instance().undoStack.top();
                        explosionQueue = std::queue<PendingExplosion>(); // Reset explosion queue
                        Game::instance().restoreFromState(state);
                        Game::instance().undoStack.pop();
                        Game::instance().redoStack.push(state); // Push to redo stack
                        gameState = GAME_STATE_PLAYING; // Go back to playing state
                    }
                } else {
                    gameState = GAME_STATE_EXIT; // Exit the game
                }
                break;
            case GAME_STATE_EXIT:
                CloseWindow(); // Exit the game
                break;
            default:
                break;
        }
    }
}