#include "PhysicsComponents.hpp"

#include "glm/gtx/norm.hpp"
#include "glm/gtc/constants.hpp"

#include "SpatialComponent.hpp"
#include "Scene/Scene.hpp"
#include "System/SpatialSystem.hpp"
#include "Util/Util.hpp"



NewtonianComponent::NewtonianComponent(GameObject & gameObject, bool isBouncy) :
    Component(gameObject),
    m_spatial(nullptr),
    m_velocity(),
    m_acceleration(),
    m_isBouncy(isBouncy)
{}

void NewtonianComponent::init() {
    if (!(m_spatial = gameObject().getSpatial())) assert(false);

    auto collisionCallback([&](const Message & msg_) {
        const CollisionNormMessage & msg(static_cast<const CollisionNormMessage &>(msg_));

        // Calculate "friction"
        float y(-glm::dot(m_velocity, msg.norm)); // speed into surface
        if (y <= 0.0) { // not trying to move into surface
            return;
        }

        if (m_isBouncy && -glm::dot(m_spatial->effectiveVelocity(), msg.norm) >= SpatialSystem::k_bounceVelThreshold) {
            m_velocity = glm::reflect(m_velocity, msg.norm) * SpatialSystem::k_elasticity;
            Scene::sendMessage<BounceMessage>(&gameObject(), msg.norm);
            return;
        }

        glm::vec3 v(m_velocity + y * msg.norm); // would-be velocity along surface
        float x(glm::length2(v)); // length of v
        if (Util::isZero(x)) {
            m_velocity = glm::vec3();
            return;
        }
        x = std::sqrt(x);
        float factor(y / x * SpatialSystem::k_coefficientOfFriction);
        if (factor >= 1.0f) { // frictional force prevents movement
            m_velocity = glm::vec3();
            return;
        }
        m_velocity = v * (1.0f - factor);
    });
    Scene::addReceiver<CollisionNormMessage>(&gameObject(), collisionCallback);
}

void NewtonianComponent::update(float dt) {
    glm::vec3 newVelocity(m_velocity + m_acceleration * dt);
    float speed2(glm::length2(newVelocity));
    if (speed2 > SpatialSystem::k_terminalVelocity *  SpatialSystem::k_terminalVelocity) {
        newVelocity *= SpatialSystem::k_terminalVelocity / std::sqrt(speed2);
    }
    glm::vec3 delta(0.5f * dt * (m_velocity + newVelocity));
    if (!Util::isZero(glm::length2(delta))) {
        m_spatial->move(delta);
    }
    m_velocity = newVelocity;
    m_acceleration = glm::vec3();
}

void NewtonianComponent::accelerate(const glm::vec3 & acceleration) {
    m_acceleration += acceleration;
}

void NewtonianComponent::addVelocity(const glm::vec3 & velocity) {
    m_velocity += velocity;
}

void NewtonianComponent::setVelocity(const glm::vec3 & velocity) {
    m_velocity = velocity;
}

void NewtonianComponent::removeAllVelocityAgainstDir(const glm::vec3 & dir) {
    m_velocity = Util::removeAllAgainst(m_velocity, dir);
}

void NewtonianComponent::removeSomeVelocityAgainstDir(const glm::vec3 & dir, float amount) {
    m_velocity = Util::removeSomeAgainst(m_velocity, dir, amount);
}



AcceleratorComponent::AcceleratorComponent(GameObject & gameObject, const glm::vec3 & acceleration) :
    Component(gameObject),
    m_newtonian(nullptr),
    m_acceleration(acceleration)
{}

void AcceleratorComponent::init() {
    if (!(m_newtonian = gameObject().getComponentByType<NewtonianComponent>())) assert(false);
}

void AcceleratorComponent::update(float dt) {
    m_newtonian->accelerate(m_acceleration);
}



GravityComponent::GravityComponent(GameObject & gameObject) :
    AcceleratorComponent(gameObject, SpatialSystem::gravity())
{}

void GravityComponent::update(float dt) {
    m_newtonian->accelerate(SpatialSystem::gravity());
}