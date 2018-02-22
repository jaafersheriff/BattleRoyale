#include "SpatialSystem.hpp"

#include "Scene/Scene.hpp"



const Vector<SpatialComponent *> & SpatialSystem::s_spatialComponents(Scene::getComponents<SpatialComponent>());
const Vector<NewtonianComponent *> & SpatialSystem::s_newtonianComponents(Scene::getComponents<NewtonianComponent>());
const Vector<AcceleratorComponent *> & SpatialSystem::s_acceleratorComponents(Scene::getComponents<AcceleratorComponent>());
glm::vec3 SpatialSystem::s_gravityDir = glm::vec3(0.0f, -1.0f, 0.0f);
float SpatialSystem::s_gravityMag = 10.0f;
float SpatialSystem::s_coefficientOfFriction = 0.5f;

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