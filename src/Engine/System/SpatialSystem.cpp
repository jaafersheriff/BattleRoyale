#include "SpatialSystem.hpp"

#include "Scene/Scene.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/SpatialComponents/PhysicsComponents.hpp"
#include "Component/SpatialComponents/AnimationComponents.hpp"



const float SpatialSystem::k_terminalVelocity = 50.0f;
const float SpatialSystem::k_coefficientOfFriction = 0.5f;
const float SpatialSystem::k_elasticity = 2.0f / 3.0f;
const float SpatialSystem::k_bounceVelThreshold = 0.5f;

const Vector<SpatialComponent *> & SpatialSystem::s_spatialComponents(Scene::getComponents<SpatialComponent>());
const Vector<NewtonianComponent *> & SpatialSystem::s_newtonianComponents(Scene::getComponents<NewtonianComponent>());
const Vector<AcceleratorComponent *> & SpatialSystem::s_acceleratorComponents(Scene::getComponents<AcceleratorComponent>());
const Vector<AnimationComponent *> & SpatialSystem::s_animationComponents(Scene::getComponents<AnimationComponent>());
glm::vec3 SpatialSystem::s_gravityDir = glm::vec3(0.0f, 0.0f, 0.0f);
float SpatialSystem::s_gravityMag = 0.0f;

void SpatialSystem::init() {

}

void SpatialSystem::update(float dt) {
    for (auto & comp : s_acceleratorComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_newtonianComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_animationComponents) {
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