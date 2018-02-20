#include "GroundComponent.hpp"

#include "Scene/Scene.hpp"
#include "System/SpatialSystem.hpp"



GroundComponent::GroundComponent(float criticalAngle) :
    m_cosCriticalAngle(std::cos(criticalAngle))
{}

void GroundComponent::init(GameObject & go) {
    Component::init(go);
    
    auto collisionCallback([&](const Message & msg_) {
        const CollisionNormMessage & msg(static_cast<const CollisionNormMessage &>(msg_));
        float dot(glm::dot(msg.norm, -SpatialSystem::get().gravityDir()));
        if (dot >= m_cosCriticalAngle) {
            m_potentialGroundNorm += msg.norm;
        }
    });
    Scene::get().addReceiver<CollisionNormMessage>(gameObject(), collisionCallback);
}

void GroundComponent::update(float dt) {
    m_groundNorm = m_potentialGroundNorm == glm::vec3() ? glm::vec3() : glm::normalize(m_potentialGroundNorm);
    m_potentialGroundNorm = glm::vec3();
}

bool GroundComponent::onGround() const {
    return m_groundNorm != glm::vec3();
}

void GroundComponent::setCriticalAngle(float criticalAngle) {
    m_cosCriticalAngle = std::cos(criticalAngle);
}