#include "PathfindingComponent.hpp"
#include "System/PathfindingSystem.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

void PathfindingComponent::init() {}

void PathfindingComponent::update(float dt) {
	glm::vec3 playerPos = player->gameObject->getSpatial()->position();
	glm::vec3 direction = glm::normalize(playerPos - gameObject->getSpatial()->position());

	gameObject->getSpatial()->move(direction * moveSpeed * dt);
}