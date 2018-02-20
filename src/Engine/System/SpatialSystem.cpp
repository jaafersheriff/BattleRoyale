#include "SpatialSystem.hpp"



std::vector<std::unique_ptr<SpatialComponent>> SpatialSystem::s_spatialComponents;
std::vector<std::unique_ptr<NewtonianComponent>> SpatialSystem::s_newtonianComponents;
std::vector<std::unique_ptr<AcceleratorComponent>> SpatialSystem::s_acceleratorComponents;
glm::vec3 SpatialSystem::s_gravityDir = glm::vec3(0.0f, -1.0f, 0.0f);
float SpatialSystem::s_gravityMag = 10.0f;
float SpatialSystem::s_coefficientOfFriction = 0.1f;

void SpatialSystem::update(float dt) {
    for (auto & comp : s_acceleratorComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_newtonianComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_spatialComponents) {
        comp->update(dt);
    }
}

void SpatialSystem::setGravity(const glm::vec3 & gravity) {
    s_gravityMag = glm::length(gravity);
    s_gravityDir = gravity / s_gravityMag;
}

void SpatialSystem::setGravityDir(const glm::vec3 & dir) {
    s_gravityDir = dir;
}

void SpatialSystem::setGravityMag(float mag) {
    s_gravityMag = mag;
}

void SpatialSystem::add(std::unique_ptr<Component> component) {
    if (dynamic_cast<SpatialComponent *>(component.get()))
        s_spatialComponents.emplace_back(static_cast<SpatialComponent *>(component.release()));
    else if (dynamic_cast<NewtonianComponent *>(component.get()))
        s_newtonianComponents.emplace_back(static_cast<NewtonianComponent *>(component.release()));
    else if (dynamic_cast<AcceleratorComponent *>(component.get()))
        s_acceleratorComponents.emplace_back(static_cast<AcceleratorComponent *>(component.release()));
    else
        assert(false);
}

void SpatialSystem::remove(Component * component) {
    if (dynamic_cast<SpatialComponent *>(component)) {
        for (auto it(s_spatialComponents.begin()); it != s_spatialComponents.end(); ++it) {
            if (it->get() == component) {
                s_spatialComponents.erase(it);
                break;
            }
        }
    }
    else if (dynamic_cast<NewtonianComponent *>(component)) {
        for (auto it(s_newtonianComponents.begin()); it != s_newtonianComponents.end(); ++it) {
            if (it->get() == component) {
                s_newtonianComponents.erase(it);
                break;
            }
        }
    }
    else if (dynamic_cast<AcceleratorComponent *>(component)) {
        for (auto it(s_acceleratorComponents.begin()); it != s_acceleratorComponents.end(); ++it) {
            if (it->get() == component) {
                s_acceleratorComponents.erase(it);
                break;
            }
        }
    }
}