#include "SpatialSystem.hpp"



std::vector<std::unique_ptr<SpatialComponent>> SpatialSystem::m_spatialComponents;
std::vector<std::unique_ptr<NewtonianComponent>> SpatialSystem::m_newtonianComponents;
std::vector<std::unique_ptr<AcceleratorComponent>> SpatialSystem::m_acceleratorComponents;
glm::vec3 SpatialSystem::m_gravityDir = glm::vec3(0.0f, -1.0f, 0.0f);
float SpatialSystem::m_gravityMag = 10.0f;
float SpatialSystem::m_coefficientOfFriction = 0.1f;

void SpatialSystem::update(float dt) {
    for (auto & comp : m_acceleratorComponents) {
        comp->update(dt);
    }
    for (auto & comp : m_newtonianComponents) {
        comp->update(dt);
    }
    for (auto & comp : m_spatialComponents) {
        comp->update(dt);
    }
}

void SpatialSystem::setGravity(const glm::vec3 & gravity) {
    m_gravityMag = glm::length(gravity);
    m_gravityDir = gravity / m_gravityMag;
}

void SpatialSystem::setGravityDir(const glm::vec3 & dir) {
    m_gravityDir = dir;
}

void SpatialSystem::setGravityMag(float mag) {
    m_gravityMag = mag;
}

void SpatialSystem::add(std::unique_ptr<Component> component) {
    if (dynamic_cast<SpatialComponent *>(component.get()))
        m_spatialComponents.emplace_back(static_cast<SpatialComponent *>(component.release()));
    else if (dynamic_cast<NewtonianComponent *>(component.get()))
        m_newtonianComponents.emplace_back(static_cast<NewtonianComponent *>(component.release()));
    else if (dynamic_cast<AcceleratorComponent *>(component.get()))
        m_acceleratorComponents.emplace_back(static_cast<AcceleratorComponent *>(component.release()));
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
    else if (dynamic_cast<NewtonianComponent *>(component)) {
        for (auto it(m_newtonianComponents.begin()); it != m_newtonianComponents.end(); ++it) {
            if (it->get() == component) {
                m_newtonianComponents.erase(it);
                break;
            }
        }
    }
    else if (dynamic_cast<AcceleratorComponent *>(component)) {
        for (auto it(m_acceleratorComponents.begin()); it != m_acceleratorComponents.end(); ++it) {
            if (it->get() == component) {
                m_acceleratorComponents.erase(it);
                break;
            }
        }
    }
}