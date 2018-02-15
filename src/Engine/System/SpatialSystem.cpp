#include "SpatialSystem.hpp"

#include "Component/SpatialComponents/SpatialComponent.hpp"



void SpatialSystem::update(float dt) {
    for (auto & comp : m_spatialComponents) {
        comp->update(dt);
    }
}

void SpatialSystem::add(std::unique_ptr<Component> component) {
    if (dynamic_cast<SpatialComponent *>(component.get()))
        m_spatialComponents.emplace_back(static_cast<SpatialComponent *>(component.release()));
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