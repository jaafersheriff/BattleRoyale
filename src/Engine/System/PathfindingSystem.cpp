#include "PathfindingSystem.hpp"

std::vector<std::unique_ptr<PathfindingComponent>> PathfindingSystem::s_pathfindingComponents;

void PathfindingSystem::update(float dt) {
    for (auto & comp : s_pathfindingComponents) {
        comp->update(dt);
    }
}

void PathfindingSystem::add(std::unique_ptr<Component> component) {
    if (dynamic_cast<PathfindingComponent *>(component.get()))
        s_pathfindingComponents.emplace_back(static_cast<PathfindingComponent *>(component.release()));
    else
        assert(false);
}

void PathfindingSystem::remove(Component * component) {
    if (dynamic_cast<PathfindingComponent *>(component)) {
        for (auto it(s_pathfindingComponents.begin()); it != s_pathfindingComponents.end(); ++it) {
            if (it->get() == component) {
                s_pathfindingComponents.erase(it);
                break;
            }
        }
    }
}