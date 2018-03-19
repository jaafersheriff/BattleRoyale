#include "AnimationComponents.hpp"

#include "SpatialComponent.hpp"



SpinAnimationComponent::SpinAnimationComponent(GameObject & gameObject, SpatialComponent & spatial, const glm::vec3 & axis, float angularSpeed) :
    AnimationComponent(gameObject, spatial),
    m_axis(glm::normalize(axis)),
    m_angularSpeed(angularSpeed)
{}

void SpinAnimationComponent::update(float dt) {
    m_spatial.rotate(glm::angleAxis(m_angularSpeed * dt, m_axis));
}