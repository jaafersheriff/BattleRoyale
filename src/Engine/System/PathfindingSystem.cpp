#include "PathfindingSystem.hpp"

std::vector<PathfindingComponent *> PathfindingSystem::s_pathfindingComponents;

void PathfindingSystem::update(float dt) {
    for (auto & comp : s_pathfindingComponents) {
        comp->update(dt);
    }
}

void PathfindingSystem::add(Component & component) {
    if (dynamic_cast<PathfindingComponent *>(&component))
        s_pathfindingComponents.emplace_back(static_cast<PathfindingComponent *>(&component));
    else
        assert(false);
}

void PathfindingSystem::remove(Component & component) {
    if (dynamic_cast<PathfindingComponent *>(&component)) {
        for (auto it(s_pathfindingComponents.begin()); it != s_pathfindingComponents.end(); ++it) {
            if (*it == &component) {
                s_pathfindingComponents.erase(it);
                break;
            }
        }
    }
}