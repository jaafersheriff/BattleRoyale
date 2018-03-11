#include "PathfindingComponent.hpp"

#include "glm/gtx/norm.hpp"

#include "System/PathfindingSystem.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Util/Util.hpp"

PathfindingComponent::PathfindingComponent(GameObject & gameObject, const GameObject & player, float ms, bool wander) :
    Component(gameObject),
    m_spatial(nullptr),
    m_player(player),
    m_moveSpeed(ms),
    m_wander(wander),
    m_wanderCurrent(0.0f, 0.0f, 0.0f),
    m_wanderCurrentWeight(0.9f),
    m_wanderWeight(0.9f)
{}

void PathfindingComponent::init() {
    if (!(m_spatial = gameObject().getSpatial())) assert(false);
}

void PathfindingComponent::update(float dt) {
    const glm::vec3 & playerPos = m_player.getSpatial()->position();
    const glm::vec3 & pos = m_spatial->position();
    glm::vec3 dir = playerPos - pos;
    if (glm::length2(dir) < 0.001f) {
        return;
    }

    if (m_wander) {
        glm::vec3 wanderNext;
        wanderNext = Util::safeNorm(glm::vec3(
            (float) ((rand() % 200) - 100), 
            (float) ((rand() % 200) - 100),
            (float) ((rand() % 200) - 100)
        ));

        m_wanderCurrent = Util::safeNorm(
            m_wanderCurrentWeight * m_wanderCurrent +
            (1.f - m_wanderCurrentWeight) * wanderNext
        );

        dir = dir * (1.f - m_wanderWeight) + m_wanderCurrent * m_wanderWeight;
    }

    gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);
}