#include "PathfindingComponent.hpp"
#include "System/PathfindingSystem.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

void Pathfinding::init() {}

void Pathfinding::update(float dt) {
	gameObject->getSpatial()->move(glm::vec3(0, 0, 1) * moveSpeed * dt);
}