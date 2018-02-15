#include "PathfindingSystem.hpp"



void PathfindingSystem::update(float dt) {
    for (auto & comp : m_pathfindingComponents) {
        comp->update(dt);
    }
}

void PathfindingSystem::add(std::unique_ptr<Component> component) {
    m_componentRefs.push_back(component.get());
    if (dynamic_cast<PathfindingComponent *>(component.get()))
        m_pathfindingComponents.emplace_back(static_cast<PathfindingComponent *>(component.release()));
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
    // remove from refs
    for (auto it(m_componentRefs.begin()); it != m_componentRefs.end(); ++it) {
        if (*it == component) {
            m_componentRefs.erase(it);
            break;
        }
    }
}