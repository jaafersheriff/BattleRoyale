#include "BounderComponent.hpp"

#include <tuple>

#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/norm.hpp"

#include "Component/SpatialComponents/SpatialComponent.hpp"



BounderComponent::BounderComponent(GameObject & gameObject, unsigned int weight) :
    Component(gameObject),
    m_spatial(nullptr),
    m_weight(weight)
{}

void BounderComponent::init() {
    if (!(m_spatial = gameObject().getSpatial())) assert(false);
}



AABBounderComponent::AABBounderComponent(GameObject & gameObject, unsigned int weight, const AABox & box) :
    BounderComponent(gameObject, weight),
    m_box(box),
    m_transBox(m_box)
{}

void AABBounderComponent::update(float dt) {
    // no rotation
    if (m_spatial->orientation() == glm::quat()) {
        m_transBox.min = m_box.min * m_spatial->scale() + m_spatial->position();
        m_transBox.max = m_box.max * m_spatial->scale() + m_spatial->position();
    }
    // is rotation
    else {
        glm::vec3 corners[8]{
            glm::vec3(m_box.min.x, m_box.min.y, m_box.min.z),
            glm::vec3(m_box.max.x, m_box.min.y, m_box.min.z),
            glm::vec3(m_box.min.x, m_box.max.y, m_box.min.z),
            glm::vec3(m_box.max.x, m_box.max.y, m_box.min.z),
            glm::vec3(m_box.min.x, m_box.min.y, m_box.max.z),
            glm::vec3(m_box.max.x, m_box.min.y, m_box.max.z),
            glm::vec3(m_box.min.x, m_box.max.y, m_box.max.z),
            glm::vec3(m_box.max.x, m_box.max.y, m_box.max.z)
        };
        const glm::mat4 & modelMat(m_spatial->modelMatrix());
        for (int i(0); i < 8; ++i) {
            corners[i] = modelMat * glm::vec4(corners[i], 1.0f);
        }
        m_transBox.min = corners[0];
        m_transBox.max = corners[0];
        for (int i(1); i < 8; ++i) {
            m_transBox.min = glm::min(m_transBox.min, corners[i]);
            m_transBox.max = glm::max(m_transBox.max, corners[i]);
        }
    }
}

bool AABBounderComponent::collide(const BounderComponent & o, glm::vec3 * delta) const {
    if (dynamic_cast<const AABBounderComponent *>(&o)) 
        return ::collide(transBox(), static_cast<const AABBounderComponent &>(o).transBox(), delta);
    else if (dynamic_cast<const SphereBounderComponent *>(&o))
        return ::collide(transBox(), static_cast<const SphereBounderComponent &>(o).transSphere(), delta);
    else if (dynamic_cast<const CapsuleBounderComponent *>(&o))
        return ::collide(transBox(), static_cast<const CapsuleBounderComponent &>(o).transCapsule(), delta);
    return false;
}

Intersect AABBounderComponent::intersect(const Ray & ray) const {
    return ::intersect(ray, m_transBox);
}

Sphere AABBounderComponent::enclosingSphere() const {
    glm::fvec3 center(0.5f * (m_transBox.max + m_transBox.min));
    float radius(glm::length(m_transBox.max - center));
    return Sphere(center, radius);
}

bool AABBounderComponent::isMovementCritical() const {
    glm::vec3 delta(m_spatial->position() - m_spatial->prevPosition());
    glm::vec3 boxRadii((m_transBox.max - m_transBox.min) * 0.5f);
    return glm::abs(delta.x) > boxRadii.x || glm::abs(delta.y) > boxRadii.y || glm::abs(delta.z) > boxRadii.z;
}



SphereBounderComponent::SphereBounderComponent(GameObject & gameObject, unsigned int weight, const Sphere & sphere) :
    BounderComponent(gameObject, weight),
    m_sphere(sphere),
    m_transSphere(m_sphere)
{}

void SphereBounderComponent::update(float dt) {
    m_transSphere.origin = m_spatial->modelMatrix() * glm::vec4(m_sphere.origin, 1.0f);
    m_transSphere.radius = glm::compMax(m_spatial->scale()) * m_sphere.radius;
}

bool SphereBounderComponent::collide(const BounderComponent & o, glm::vec3 * delta) const {
    if (dynamic_cast<const AABBounderComponent *>(&o)) {
        bool res(::collide(static_cast<const AABBounderComponent &>(o).transBox(), transSphere(), delta));
        if (delta) *delta *= -1.0f;
        return res;
    }
    else if (dynamic_cast<const SphereBounderComponent *>(&o))
        return ::collide(transSphere(), static_cast<const SphereBounderComponent &>(o).transSphere(), delta);
    else if (dynamic_cast<const CapsuleBounderComponent *>(&o))
        return ::collide(transSphere(), static_cast<const CapsuleBounderComponent &>(o).transCapsule(), delta);
    return false;
}

Intersect SphereBounderComponent::intersect(const Ray & ray) const {
    return ::intersect(ray, m_transSphere);
}

Sphere SphereBounderComponent::enclosingSphere() const {
    return m_transSphere;
}

bool SphereBounderComponent::isMovementCritical() const {
    glm::vec3 delta(m_spatial->position() - m_spatial->prevPosition());
    return glm::length2(delta) > m_transSphere.radius * m_transSphere.radius;
}



CapsuleBounderComponent::CapsuleBounderComponent(GameObject & gameObject, unsigned int weight, const Capsule & capsule) :
    BounderComponent(gameObject, weight),
    m_capsule(capsule),
    m_transCapsule(m_capsule)
{}

void CapsuleBounderComponent::update(float dt) {
    m_transCapsule.center = m_spatial->modelMatrix() * glm::vec4(m_capsule.center, 1.0f);
    const glm::vec3 & scale(m_spatial->scale());
    m_transCapsule.radius = glm::max(scale.x, scale.z) * m_capsule.radius;
    m_transCapsule.height = glm::max(0.0f, scale.y * (m_capsule.height + 2.0f * m_capsule.radius) - 2.0f * m_transCapsule.radius);
}

bool CapsuleBounderComponent::collide(const BounderComponent & o, glm::vec3 * delta) const {
    if (dynamic_cast<const AABBounderComponent *>(&o)) {
        bool res(::collide(static_cast<const AABBounderComponent &>(o).transBox(), transCapsule(), delta));
        if (delta) *delta *= -1.0f;
        return res;
    }
    else if (dynamic_cast<const SphereBounderComponent *>(&o)) {
        bool res(::collide(static_cast<const SphereBounderComponent &>(o).transSphere(), transCapsule(), delta));
        if (delta) *delta *= -1.0f;
        return res;
    }
    else if (dynamic_cast<const CapsuleBounderComponent *>(&o))
        return ::collide(transCapsule(), static_cast<const CapsuleBounderComponent &>(o).transCapsule(), delta);
    return false;
}

Intersect CapsuleBounderComponent::intersect(const Ray & ray) const {
    return ::intersect(ray, m_transCapsule);
}

Sphere CapsuleBounderComponent::enclosingSphere() const {
    return Sphere(m_transCapsule.center, m_transCapsule.height * 0.5f + m_transCapsule.radius);
}

bool CapsuleBounderComponent::isMovementCritical() const {
    glm::vec3 delta(m_spatial->position() - m_spatial->prevPosition());
    float dxz2(delta.x * delta.x + delta.z * delta.z);
    float r2(m_transCapsule.radius * m_transCapsule.radius);
    if (dxz2 > r2) {
        return true;
    }
    float h_2(m_transCapsule.height * 0.5f);
    if (delta.y > h_2) {
        float dy(delta.y - h_2);
        if (dxz2 + dy * dy > r2) {
            return true;
        }
    }
    else if (delta.y < h_2) {
        float dy(delta.y + h_2);
        if (dxz2 + dy * dy > r2) {
            return true;
        }
    }
    return false;
}