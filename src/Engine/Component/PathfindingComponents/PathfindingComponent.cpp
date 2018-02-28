#include "PathfindingComponent.hpp"

#include "glm/gtx/norm.hpp"

#include "System/PathfindingSystem.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

PathfindingComponent::PathfindingComponent(GameObject & player, float ms) :
    m_spatial(nullptr),
    m_player(&player),
    m_moveSpeed(ms)
{}

void PathfindingComponent::init(GameObject & go) {
    Component::init(go);
    if (!(m_spatial = gameObject()->getSpatial())) assert(false);
}

void PathfindingComponent::update(float dt) {
    const glm::vec3 & playerPos = m_player->getSpatial()->position();
    const glm::vec3 & pos = m_spatial->position();
    glm::vec3 dir = playerPos - pos;
    if (glm::length2(dir) < 0.001f) {
        return;
    }


    const int xdir[] = {1, 1, 0, -1, -1, -1, 0, 1};
    const int zdir[] = {0, 1, 1, 1, 0, -1, -1, -1};


    gameObject()->getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);
}