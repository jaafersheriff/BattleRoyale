#include "PathfindingComponent.hpp"
#include "System/PathfindingSystem.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

void PathfindingComponent::init() {}

void PathfindingComponent::update(float dt) {
	gameObject->getSpatial()->move(glm::vec3(0, 0, 1) * moveSpeed * dt);
}