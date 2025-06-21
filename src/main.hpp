#include <vector>
#include <numbers>
#include <cmath>
#include <list>
#include <string>
#include <queue>
#include "raylib.h"
#include "raymath.h"
#define SPACING 100
#define depth 100
#define MIN_SHAKE_STRENGTH 1.0f
#define MAX_SHAKE_STRENGTH 5.0f
#define ExplosionDuration 30.0f
#define SphereRadius 20
#define SPHERE_SEGMENTS 64




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

class Game {
public:
    std::vector<std::vector<Cell>> Board;
    int rows;
    int cols;
    int playerCount;
    std::list<int> Players;
    int currentPlayer;
    int turns;
    bool pendingTurnChange;
    bool skipExplosions;

    static Game& instance();
    void initialize(int r, int c, int p);
    void drawGame() const;
    int getPlayer() const;
    void changePlayer();
    void press(float x, float y);
    int intermediaryGameEndCheck();
    int gameEndCheck();
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
};


extern std::queue<PendingExplosion> explosionQueue;
extern std::queue<PendingExplosion> nextQueue;

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