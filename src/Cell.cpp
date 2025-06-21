#include "main.hpp"
Cell::Cell(int x_, int y_) : x(x_), y(y_), l(0), p(0) {
    SphereMesh = GenMeshSphere(SphereRadius, SPHERE_SEGMENTS, SPHERE_SEGMENTS);
    coreMaterial = LoadMaterialDefault();
    coreMaterial.shader = coreShader;
    auraMaterial = LoadMaterialDefault();
    auraMaterial.shader = auraShader;
}

void Cell::drawCell() const {
    if (p < 1 || p >= colors.size()) return;

    Color cellColor = PlayerIDtoColor(p);
    // increase color brightness based on level
    
    
    Vector3 rgb = {(float)cellColor.r / 255.0f, (float)cellColor.g / 255.0f, (float)cellColor.b / 255.0f};

    float cx = x * SPACING + SPACING / 2;
    float cy = y * SPACING + SPACING / 2;
    float cz = depth / 2;

    float shakeStrength = shakeStengthFromLevel(l, neighbors.size());
    float time = GetTime();

    // Define positions for 1, 2, and 3 spheres (like atoms)
    std::vector<Vector3> offsets;
    float bondDist = 22.0f + shakeStrength * 0.5f; // distance between spheres

    if (l == 1) {
        // Single sphere at center
        offsets.push_back({0, 0, 0});
    } else if (l == 2) {
        // Two spheres, side by side (horizontal bond)
        offsets.push_back({-bondDist / 2, 0, 0});
        offsets.push_back({bondDist / 2, 0, 0});
    } else if (l == 3) {
        // H2O-like: one center, two at ~104.5 degrees (water bond angle)
        float angle = 52.25f * DEG2RAD; // half of 104.5°
        offsets.push_back({0, 0, 0}); // Oxygen (center)
        offsets.push_back({cosf(angle) * bondDist, sinf(angle) * bondDist, 0});
        offsets.push_back({cosf(-angle) * bondDist, sinf(-angle) * bondDist, 0});
    } else {
        // For l > 3, arrange in a circle
        for (int i = 0; i < l; i++) {
            float theta = (2 * PI * i) / l;
            offsets.push_back({cosf(theta) * bondDist, sinf(theta) * bondDist, 0});
        }
    }

    for (int i = 0; i < l; i++) {
        float t = getFrameCount() * 0.05f + i * 10;
        float offsetX = offsets[i].x + shakeStrength * sinf(t + i);
        float offsetY = offsets[i].y + shakeStrength * cosf(t + i * 1.3f);
        float offsetZ = offsets[i].z + shakeStrength * sinf(t + i * 0.7f);

        Vector3 pos = {cx + offsetX, cy + offsetY, cz + offsetZ};
        Matrix transform = MatrixTranslate(pos.x, pos.y, pos.z);
        Matrix mvp = MatrixMultiply(GetCameraMatrix(camera), transform); // adjust as needed

        // === First Pass: Solid Core ===
        SetShaderValueMatrix(coreShader, mvpLocCore, mvp);
        SetShaderValue(coreShader, baseColorLocCore, &rgb, SHADER_UNIFORM_VEC3);
        DrawMesh(SphereMesh, coreMaterial, transform);

        // === Second Pass: Aura ===
        BeginBlendMode(BLEND_ALPHA); // or BLEND_ADDITIVE
        SetShaderValueMatrix(auraShader, mvpLocAura, mvp);
        SetShaderValue(auraShader, baseColorLocAura, &rgb, SHADER_UNIFORM_VEC3);
        SetShaderValue(auraShader, timeLocAura, &time, SHADER_UNIFORM_FLOAT);
        DrawMesh(SphereMesh, auraMaterial, transform);
        EndBlendMode(); // optional
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