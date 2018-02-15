#include "PathfindingComponent.hpp"
#include "System/PathfindingSystem.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

PathfindingComponent::PathfindingComponent(CameraComponent & cc, float ms) :
	player(&cc),
	moveSpeed(ms)
{}

void PathfindingComponent::init() {}

void PathfindingComponent::update(float dt) {
	glm::vec3 playerPos = player->getGameObject()->getSpatial()->position();
	glm::vec3 direction = glm::normalize(playerPos - gameObject->getSpatial()->position());

	gameObject->getSpatial()->move(direction * moveSpeed * dt);
}