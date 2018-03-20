#include "AnimationComponents.hpp"

#include "SpatialComponent.hpp"
#include "Util/Util.hpp"
#include "Scene/Scene.hpp"



SpinAnimationComponent::SpinAnimationComponent(GameObject & gameObject, SpatialComponent & spatial, const glm::vec3 & axis, float angularSpeed) :
    AnimationComponent(gameObject, spatial),
    m_axis(glm::normalize(axis)),
    m_angularSpeed(angularSpeed)
{}

void SpinAnimationComponent::update(float dt) {
    m_spatial.rotate(glm::angleAxis(m_angularSpeed * dt, m_axis));
}



ScaleToAnimationComponent::ScaleToAnimationComponent(GameObject & gameObject, SpatialComponent & spatial, const glm::vec3 & scale, float speed) :
    AnimationComponent(gameObject, spatial),
    m_scale(scale),
    m_speed(speed)
{}

void ScaleToAnimationComponent::update(float dt) {
    const glm::vec3 & currentScale(m_spatial.relativeScale());
    if (Util::isEqual(currentScale, m_scale)) {
        Scene::removeComponent<AnimationComponent>(*this);
        return;
    }

    glm::vec3 factor(glm::clamp(m_scale / m_spatial.relativeScale(), 1.0f - m_speed * dt, 1.0f + m_speed * dt));
    m_spatial.scaleBy(factor);
}