#ifndef MAIN_HPP
#define MAIN_HPP
#include <vector>
#include <numbers>
#include <cmath>
#include <string>
#include <queue>
#include <stack>
#include <algorithm>
#include <map>
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"

#define MIN_ROWS 5
#define MAX_ROWS 10
#define MIN_COLS 5
#define MAX_COLS 10
#define MIN_PLAYERS 1
#define MAX_PLAYERS 6
#define MIN_CPU_PLAYERS 1
#define MAX_CPU_PLAYERS 5
#define SPACING 100
#define depth 100
#define SphereRadius 20
#define SPHERE_SEGMENTS 64
#define MIN_SHAKE_STRENGTH 1.0f
#define MAX_SHAKE_STRENGTH 5.0f
#define ExplosionDuration 30.0f




enum GAME_STATE {
    GAME_STATE_START,
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_GAME_OVER,
    GAME_STATE_EXIT
};


extern int frameCount;
extern GAME_STATE gameState;
extern std::vector<Color> colors;
extern std::vector<std::string> playerNames;

void incrementFrameCount();
int getFrameCount();

Color PlayerIDtoColor(int playerID);
std::string PlayerIDtoName(int playerID);



class Cell {
public:
    int x, y, l, p;
    Mesh SphereMesh;
    Material coreMaterial;
    Material auraMaterial;
    std::vector<Cell*> neighbors;

    Cell(int x_, int y_);
    void drawCell() const;
    void addNeighbor(Cell* neighbor);
    void explode(int playerID);
    bool incr(int playerID, bool player);
    Vector3 getCenter() const;
};

class GameState; // Forward declaration

class Player{
public:
    int id;
    bool isCPU;
    
    Player(int id_, bool isCPU) : id(id_), isCPU(isCPU) {}
    int getID() const { return id; }
    bool isCPUPlayer() const { return isCPU; }
    std::pair<int, int> CPUTurn();
    std::pair<GameState, int> simulateTurn(GameState state, int x, int y);
};

class GameState {
public:
    std::vector<std::vector<Cell>> Board;
    std::vector<Player> Players;
    int currentPlayer;
    bool pendingTurnChange;
    int turns;
    bool skipExplosions;
    GameState(const std::vector<std::vector<Cell>>& board, const std::vector<Player>& players, int currentPlayer_, bool pendingTurnChange_, int turns_, bool skipExplosions_)
        : Board(board), Players(players), currentPlayer(currentPlayer_), pendingTurnChange(pendingTurnChange_), turns(turns_), skipExplosions(skipExplosions_) {
            linkNeighbours();
    }

    void linkNeighbours() {
        for (int i = 0; i < Board.size(); ++i) {
            for (int j = 0; j < Board[i].size(); ++j) {
                Board[i][j].neighbors.clear(); // Clear existing neighbors
                if (i > 0) Board[i][j].addNeighbor(&Board[i - 1][j]); // Up
                if (i < Board.size() - 1) Board[i][j].addNeighbor(&Board[i + 1][j]); // Down
                if (j > 0) Board[i][j].addNeighbor(&Board[i][j - 1]); // Left
                if (j < Board[i].size() - 1) Board[i][j].addNeighbor(&Board[i][j + 1]); // Right
            }
        }
    }
};

class Game {
public:
    std::vector<std::vector<Cell>> Board;
    std::stack<GameState> undoStack;
    std::stack<GameState> redoStack;
    int rows;
    int cols;
    int playerCount;
    std::vector<Player> Players;
    int currentPlayer;
    int turns;
    bool pendingTurnChange;
    bool skipExplosions;

    static Game& instance();
    void initialize(int r, int c, int p, int cpu);
    void drawGame() const;
    int getPlayer() const;
    Player getCurrentPlayer() const {
        if (currentPlayer < 0 || currentPlayer >= Players.size()) {
            return Player(-1, false); // Invalid player
        }
        return Players[currentPlayer];
    }
    void changePlayer();
    void press(float x, float y);
    void CPUpress(int X, int Y);
    void eliminatePlayer(int playerID);
    int intermediaryGameEndCheck();
    int gameEndCheck();
    GameState getCurrentState() const {
        return GameState(Board, Players, currentPlayer, pendingTurnChange, turns, skipExplosions);
    }
    void linkNeighbours() {
        for (int i = 0; i < Board.size(); ++i) {
            for (int j = 0; j < Board[i].size(); ++j) {

                Board[i][j].neighbors.clear(); // Clear existing neighbors
                if (i > 0) Board[i][j].addNeighbor(&Board[i - 1][j]); // Up
                if (i < Board.size() - 1) Board[i][j].addNeighbor(&Board[i + 1][j]); // Down
                if (j > 0) Board[i][j].addNeighbor(&Board[i][j - 1]); // Left
                if (j < Board[i].size() - 1) Board[i][j].addNeighbor(&Board[i][j + 1]); // Right
            }
        }
    }
    void restoreFromState(const GameState& state) {
        Board = state.Board;
        Players = state.Players;
        currentPlayer = state.currentPlayer;
        pendingTurnChange = state.pendingTurnChange;
        turns = state.turns;
        skipExplosions = state.skipExplosions;
        linkNeighbours();
    }
    Vector2 screenToGrid(Vector2 screenPos) const;
private:
    Game();
    ~Game();
    Game(const Game&) = delete; // Disable copy constructor
    Game& operator=(const Game&) = delete; // Disable assignment operator
    Game(Game&&) = delete; // Disable move constructor
    Game& operator=(Game&&) = delete; // Disable move assignment operator

};

class PendingExplosion {
public:
    Cell* source;
    Cell* target;
    int playerID;
    int frameStarted;

    PendingExplosion(Cell* source_, Cell* target_, int playerID_, int frameStarted_);
    bool isComplete();
    void update();
    void complete();
    void resetExplosionQueue();
};


extern std::queue<PendingExplosion> explosionQueue;

extern Rectangle menuRect;
extern Rectangle rowSliderRect;
extern Rectangle colSliderRect;
extern Rectangle playerSliderRect;
extern Rectangle buttonRect;
extern Rectangle restartButtonRect;
extern Rectangle exitButtonRect;
extern Rectangle undoButtonRect;
extern Rectangle redoButtonRect;
extern Rectangle CPUSliderRect;

extern float rowValue;
extern float colValue;
extern float playerValue;
extern float cpuValue;

extern Shader coreShader;
extern Shader auraShader;
extern int baseColorLocCore;
extern int baseColorLocAura;
extern int timeLocAura;
extern int mvpLocCore;
extern int mvpLocAura;
extern Camera3D camera;
float shakeStengthFromLevel(int level, int neighborCount);

void mousePressed();

void drawStart();
void drawMenu();
void drawPlaying();
void drawGameOver();
void drawExit();
void drawValidCursor();
void drawExplosions();
void initializeCamera();
void moveCameraviaRightClick();
void resetCameraviaMiddleClick();
void zoomCameraViaScroll();
void resizeAssets(int screenWidth, int screenHeight);

#endif