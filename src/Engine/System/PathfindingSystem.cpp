#include "PathfindingSystem.hpp"

std::vector<std::unique_ptr<PathfindingComponent>> PathfindingSystem::m_pathfindingComponents;

void PathfindingSystem::update(float dt) {
    for (auto & comp : m_pathfindingComponents) {
        comp->update(dt);
    }
}

void PathfindingSystem::add(std::unique_ptr<Component> component) {
    if (dynamic_cast<PathfindingComponent *>(component.get()))
        m_pathfindingComponents.emplace_back(static_cast<PathfindingComponent *>(component.release()));
    else
        assert(false);
}

void PathfindingSystem::remove(Component * component) {
    if (dynamic_cast<PathfindingComponent *>(component)) {
        for (auto it(m_pathfindingComponents.begin()); it != m_pathfindingComponents.end(); ++it) {
            if (it->get() == component) {
                m_pathfindingComponents.erase(it);
                break;
            }
        }
    }
}