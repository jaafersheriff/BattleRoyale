#include "PostCollisionSystem.hpp"


void PostCollisionSystem::update(float dt) {
    for (auto & comp : m_groundComponents) {
        comp->update(dt);
    }
}

void PostCollisionSystem::add(std::unique_ptr<Component> component) {
    m_componentRefs.push_back(component.get());
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
    // remove from refs
    for (auto it(m_componentRefs.begin()); it != m_componentRefs.end(); ++it) {
        if (*it == component) {
            m_componentRefs.erase(it);
            break;
        }
    }
}