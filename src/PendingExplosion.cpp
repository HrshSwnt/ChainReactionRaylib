#include "main.hpp"

PendingExplosion::PendingExplosion(Cell* source_, Cell* target_, int playerID_, int frameStarted_)
    : source(source_), target(target_), playerID(playerID_), frameStarted(frameStarted_) {}

bool PendingExplosion::isComplete() {
    return (getFrameCount() - frameStarted) >= ExplosionDuration;
}


void PendingExplosion::update() {
    float progress = static_cast<float>(getFrameCount() - frameStarted) / ExplosionDuration;
    if (progress > 1.0f) {
        progress = 1.0f;
    } else if (progress < 0.0f) {
        progress = 0.0f;
    }
    Vector3 sourceCenter = source->getCenter();
    Vector3 targetCenter = target->getCenter();
    Vector3 positionCenter = {
        sourceCenter.x + (targetCenter.x - sourceCenter.x) * progress,
        sourceCenter.y + (targetCenter.y - sourceCenter.y) * progress,
        sourceCenter.z + (targetCenter.z - sourceCenter.z) * progress
    };

    DrawSphere(positionCenter, SphereRadius, PlayerIDtoColor(playerID));

}

void PendingExplosion::complete() {
    target->incr(playerID, false);
    source->p = 0;
}