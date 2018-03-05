#include "PathfindingComponent.hpp"

#include "glm/gtx/norm.hpp"
// Wander code start
#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"
// Wander code end

#include "System/PathfindingSystem.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Util/Util.hpp"

PathfindingComponent::PathfindingComponent(GameObject & gameObject, GameObject & player, float ms) :
    Component(gameObject),
    m_spatial(nullptr),
    m_player(&player),
    m_moveSpeed(ms)
{
    // Wander code start
    wanderCurrent = glm::vec3(0.f, 0.f, 0.f);
    
    wanderCurrentWeight = .999f;
    wanderWeight = .9325f;
    // Wander code end
}

void PathfindingComponent::init() {
    if (!(m_spatial = gameObject().getSpatial())) assert(false);
}

void PathfindingComponent::update(float dt) {
    const glm::vec3 & playerPos = m_player->getSpatial()->position();
    const glm::vec3 & pos = m_spatial->position();
    glm::vec3 dir = playerPos - pos;
    if (glm::length2(dir) < 0.001f) {
        return;
    }

    // Wander code start
    glm::vec3 wanderNext;
    wanderNext = glm::vec3(
        (float) ((rand() % 200) - 100), 
        (float) ((rand() % 200) - 100),
        (float) ((rand() % 200) - 100)
    );

    wanderCurrent = Util::safeNorm(
        wanderCurrentWeight * wanderCurrent +
        (1.f - wanderCurrentWeight) * wanderNext
    );

    dir = dir * (1.f - wanderWeight) + wanderCurrent * wanderWeight;
    // Wander code end

    gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);
}