#include "SpatialSystem.hpp"

#include "Component/SpatialComponents/SpatialComponent.hpp"



void SpatialSystem::update(float dt) {
    for (auto & comp : m_spatialComponents) {
        comp->update(dt);
    }
}
    
void SpatialSystem::setObjPosition(GameObject & go, const glm::vec3 & position) {
    go.getSpatial()->setPosition(position);
    
}

void SpatialSystem::moveObj(GameObject & go, const glm::vec3 & delta) {
    go.getSpatial()->move(delta);
}

void SpatialSystem::setObjScale(GameObject & go, const glm::vec3 & scale) {
    go.getSpatial()->setScale(scale);
}

void SpatialSystem::scaleObj(GameObject & go, const glm::vec3 & factor) {
    go.getSpatial()->scale(factor);
}

void SpatialSystem::setObjRotation(GameObject & go, const glm::mat3 & rot) {
    go.getSpatial()->setRotation(rot);
}
    
void SpatialSystem::rotateObj(GameObject & go, const glm::mat3 & mat) {
    go.getSpatial()->rotate(mat);
}

void SpatialSystem::add(std::unique_ptr<Component> component) {
    m_componentRefs.push_back(component.get());
    if (dynamic_cast<SpatialComponent *>(component.get()))
        m_spatialComponents.emplace_back(static_cast<SpatialComponent *>(component.release()));
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