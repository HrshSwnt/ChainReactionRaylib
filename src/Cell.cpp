#include "main.hpp"
Cell::Cell(int x_, int y_) : x(x_), y(y_), l(0), p(0) {}

void Cell::drawCell() const {
    if (p < 1 || p >= colors.size()) {
        return; // Invalid player ID, do not draw
    }
    Color cellColor = PlayerIDtoColor(p);
    float cx = x * SPACING + SPACING / 2;
    float cy = y * SPACING + SPACING / 2;
    float cz = depth / 2;

    float shakeStrength = shakeStengthFromLevel(l, neighbors.size());
    for (int i = 0; i < l; i++){
        float t = GetFrameTime() * 0.05f + i * 10;
        float offsetX = shakeStrength * sinf(t + i );
        float offsetY = shakeStrength * cosf(t + i * 1.3f);
        float offsetZ = -i * 15 + shakeStrength * sinf(t + i * 0.7f);
        DrawSphere(
            {cx + offsetX, cy + offsetY, cz + offsetZ},
            SphereRadius,
            cellColor
        );
    }
}

void Cell::addNeighbor(Cell* neighbor) {
    neighbors.push_back(neighbor);
}

void Cell::explode(int playerID) {
    if (l == neighbors.size()) {
        l = 0;
        for (Cell* neighbor : neighbors) {
            explosionQueue.push(PendingExplosion(this, neighbor, playerID, getFrameCount() ));
        }
    }
}

bool Cell::incr(int playerID, bool player) {
    if (l > 0 && p != playerID && player) {
        return false; // Cannot increment if already occupied by another player
    } else {
        if (l == 0 || !player) {
            p = playerID; // Set player ID if not already set
        }
        l++;
        explode(playerID);
        return true; // Increment successful
    }

}

Vector3 Cell::getCenter() const {
    return {static_cast<float>(x) * SPACING + SPACING / 2, static_cast<float>(y) * SPACING + SPACING / 2, depth / 2};
}