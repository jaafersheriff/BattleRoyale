#include "SpatialSystem.hpp"

#include "Component/SpatialComponents/SpatialComponent.hpp"



void SpatialSystem::update(float dt) {
    for (auto & comp : m_spatialComponents) {
        comp->update(dt);
    }
}

void SpatialSystem::add(std::unique_ptr<Component> component) {
    m_componentRefs.push_back(component.get());
    if (dynamic_cast<SpatialComponent *>(component.get()))
        m_spatialComponents.emplace_back(static_cast<SpatialComponent *>(component.release()));
    else
        assert(false);
}

void SpatialSystem::remove(Component * component) {
    if (dynamic_cast<SpatialComponent *>(component)) {
        for (auto it(m_spatialComponents.begin()); it != m_spatialComponents.end(); ++it) {
            if (it->get() == component) {
                m_spatialComponents.erase(it);
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