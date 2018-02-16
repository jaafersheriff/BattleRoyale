#include "PhysicsComponents.hpp"

#include "glm/gtx/norm.hpp"

#include "SpatialComponent.hpp"
#include "Scene/Scene.hpp"
#include "System/SpatialSystem.hpp"



NewtonianComponent::NewtonianComponent(SpatialComponent & spatial, float maxSpeed) :
    Component(),
    m_spatial(spatial),
    m_velocity(),
    m_acceleration(),
    m_maxSpeed(maxSpeed)
{}

void NewtonianComponent::init() {
    auto velocityResetCallback([&](const Message & msg) {
        m_velocity = glm::vec3();
    });
    Scene::get().addReceiver<SpatialPositionSetMessage>(m_gameObject, velocityResetCallback);
}

void NewtonianComponent::update(float dt) {
    glm::vec3 newVelocity(m_velocity + m_acceleration * dt);
    float speed2(glm::length2(newVelocity));
    if (speed2 > m_maxSpeed * m_maxSpeed) {
        newVelocity *= m_maxSpeed / std::sqrt(speed2);
    }
    glm::vec3 delta(0.5f * dt * (m_velocity + newVelocity));
    if (!Util::isZero(glm::length2(delta))) {
        m_spatial.move(delta);
    }
    m_velocity = newVelocity;
    m_acceleration = glm::vec3();
}

void NewtonianComponent::accelerate(const glm::vec3 & acceleration) {
    m_acceleration += acceleration;
}



AcceleratorComponent::AcceleratorComponent(NewtonianComponent & spatial, const glm::vec3 & acceleration) :
    Component(),
    m_newtonian(spatial),
    m_acceleration(acceleration)
{}

void AcceleratorComponent::update(float dt) {
    m_newtonian.accelerate(m_acceleration);
}



GravityComponent::GravityComponent(NewtonianComponent & newtonian) :
    AcceleratorComponent(newtonian, SpatialSystem::get().gravity())
{}

void GravityComponent::update(float dt) {
    m_newtonian.accelerate(SpatialSystem::get().gravity());
}