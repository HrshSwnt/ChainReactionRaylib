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
    DrawText("Menu", GetScreenWidth()/2, GetScreenHeight()/2, 20, WHITE);
    DrawText("Enter the number of rows", GetScreenWidth()/2, GetScreenHeight()/2 + 30, 20, WHITE);
    DrawText("Enter the number of columns", GetScreenWidth()/2, GetScreenHeight()/2 + 60, 20, WHITE);
    DrawText("Enter the number of players", GetScreenWidth()/2, GetScreenHeight()/2 + 90, 20, WHITE);
    DrawText("Click to start the game", GetScreenWidth()/2, GetScreenHeight()/2 + 120, 20, WHITE);
}

void drawPlaying() {
    DrawText("Game Time", GetScreenWidth()/2, 0, 20, WHITE);
    std::string playerName = PlayerIDtoName(Game::instance().getPlayer()); // Assuming player ID 1 is the current player
    DrawText(("Current Player: " + playerName).c_str(), GetScreenWidth()/2, 30, 20, WHITE);

    Game::instance().drawGame();
}

void drawGameOver() {
    drawPlaying();
    int winner = Game::instance().getPlayer();
    DrawText(("Game Over! Player " + PlayerIDtoName(winner) + " wins!").c_str(), GetScreenWidth()/2, GetScreenHeight()-50, 20, WHITE);
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
        }
    } else {
        // Draw a red circle if the cursor is out of bounds
        Vector2 screenPos = GetMousePosition();
        DrawCircleV(screenPos, 10, RED);
    }
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
        {xOffset, yOffset, -std::max(static_cast<float>(rows), static_cast<float>(cols)) * SPACING}, // Position above the center
        {xOffset, yOffset, zOffset}, // Target is the center of the grid
        {0, 1, 0}, // Up vector
        80.0f // FOV
    };
}

void mousePressed(){
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        switch (gameState) {
            case GAME_STATE_START:
                gameState = GAME_STATE_MENU;
                break;
            case GAME_STATE_MENU:
                initializeCamera(6, 6); // Initialize camera based on user input
                Game::instance().initialize(6, 6, 3); // Example initialization
                gameState = GAME_STATE_PLAYING;
                break;
            case GAME_STATE_PLAYING:
                if (explosionQueue.empty()) {
                    Game::instance().press(mousePos.x, mousePos.y);
                } else {
                    Game::instance().skipExplosions = true; // Skip explosions if any are pending
                }
                break;
            case GAME_STATE_GAME_OVER:
                gameState = GAME_STATE_EXIT; // Or restart the game
                break;
            case GAME_STATE_EXIT:
                CloseWindow(); // Exit the game
                break;
            default:
                break;
        }
    }
}