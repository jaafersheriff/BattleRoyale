#include "PostCollisionSystem.hpp"

std::vector<std::unique_ptr<GroundComponent>> PostCollisionSystem::m_groundComponents;

void PostCollisionSystem::update(float dt) {
    for (auto & comp : m_groundComponents) {
        comp->update(dt);
    }
}

void PostCollisionSystem::add(std::unique_ptr<Component> component) {
    if (dynamic_cast<GroundComponent *>(component.get()))
        m_groundComponents.emplace_back(static_cast<GroundComponent *>(component.release()));
    else
        assert(false);
}

void PostCollisionSystem::remove(Component * component) {
    if (dynamic_cast<GroundComponent *>(component)) {
        for (auto it(m_groundComponents.begin()); it != m_groundComponents.end(); ++it) {
            if (it->get() == component) {
                m_groundComponents.erase(it);
                break;
            }
        }
    }
}