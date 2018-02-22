#include "PathfindingSystem.hpp"

#include "Scene/Scene.hpp"

const Vector<PathfindingComponent *> & PathfindingSystem::s_pathfindingComponents(Scene::getComponents<PathfindingComponent>());

void PathfindingSystem::update(float dt) {
    for (auto & comp : s_pathfindingComponents) {
        comp->update(dt);
    }
}