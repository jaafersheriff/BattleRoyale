#include "PathfindingComponent.hpp"

#include "glm/gtx/norm.hpp"

#include "System/PathfindingSystem.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

PathfindingComponent::PathfindingComponent(GameObject & player, float ms) :
    player(player),
    moveSpeed(ms)
{}

void PathfindingComponent::init() {}

void PathfindingComponent::update(float dt) {
    const glm::vec3 & playerPos = player.getSpatial()->position();
    const glm::vec3 & pos = m_gameObject->getSpatial()->position();
    glm::vec3 dir = playerPos - pos;
    if (glm::length2(dir) < 0.001f) {
        return;
    }
    
    m_gameObject->getSpatial()->move(glm::normalize(dir) * moveSpeed * dt);
}