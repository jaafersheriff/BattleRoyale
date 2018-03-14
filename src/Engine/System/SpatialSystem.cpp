#include "SpatialSystem.hpp"

#include "Scene/Scene.hpp"



const float SpatialSystem::k_terminalVelocity = 50.0f;
const float SpatialSystem::k_coefficientOfFriction = 0.5f;
const float SpatialSystem::k_elasticity = 0.75f;
const float SpatialSystem::k_bounceCosThreshold = std::cos(glm::radians(85.0f));

const Vector<SpatialComponent *> & SpatialSystem::s_spatialComponents(Scene::getComponents<SpatialComponent>());
const Vector<NewtonianComponent *> & SpatialSystem::s_newtonianComponents(Scene::getComponents<NewtonianComponent>());
const Vector<AcceleratorComponent *> & SpatialSystem::s_acceleratorComponents(Scene::getComponents<AcceleratorComponent>());
glm::vec3 SpatialSystem::s_gravityDir = glm::vec3(0.0f, 0.0f, 0.0f);
float SpatialSystem::s_gravityMag = 0.0f;

void SpatialSystem::update(float dt) {
    for (auto & comp : s_acceleratorComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_newtonianComponents) {
        comp->update(dt);
    }
}

void SpatialSystem::setGravity(const glm::vec3 & gravity) {
    if (gravity == glm::vec3()) {
        s_gravityDir = glm::vec3();
        s_gravityMag = 0.0f;
    }
    else {
        s_gravityMag = glm::length(gravity);
        s_gravityDir = gravity / s_gravityMag;
    }
}

void SpatialSystem::setGravityDir(const glm::vec3 & dir) {
    s_gravityDir = dir;
}

void SpatialSystem::setGravityMag(float mag) {
    s_gravityMag = mag;
}