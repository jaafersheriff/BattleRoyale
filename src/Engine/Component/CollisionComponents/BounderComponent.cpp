#include "BounderComponent.hpp"

#include <tuple>

#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/norm.hpp"

#include "Component/SpatialComponents/SpatialComponent.hpp"



BounderComponent::BounderComponent(GameObject & gameObject, unsigned int weight) :
    Component(gameObject),
    m_spatial(nullptr),
    m_weight(weight),
    m_isChange(false)
{}

void BounderComponent::init() {
    if (!(m_spatial = gameObject().getSpatial())) assert(false);
}



AABox AABBounderComponent::transformAABox(const AABox & box, const glm::mat4 & transMat) {
    glm::vec3 corners[8]{
        glm::vec3(box.min.x, box.min.y, box.min.z),
        glm::vec3(box.max.x, box.min.y, box.min.z),
        glm::vec3(box.min.x, box.max.y, box.min.z),
        glm::vec3(box.max.x, box.max.y, box.min.z),
        glm::vec3(box.min.x, box.min.y, box.max.z),
        glm::vec3(box.max.x, box.min.y, box.max.z),
        glm::vec3(box.min.x, box.max.y, box.max.z),
        glm::vec3(box.max.x, box.max.y, box.max.z)
    };
    for (int i(0); i < 8; ++i) {
        corners[i] = glm::vec3(transMat * glm::vec4(corners[i], 1.0f));
    }

    AABox transBox(corners[0], corners[0]);
    for (int i(1); i < 8; ++i) {
        transBox.min = glm::min(transBox.min, corners[i]);
        transBox.max = glm::max(transBox.max, corners[i]);
    }
    return transBox;
}

AABox AABBounderComponent::transformAABox(const AABox & box, const glm::vec3 & position, const glm::vec3 & scale) {
    return AABox(
        box.min * scale + position,
        box.max * scale + position
    );
}

AABBounderComponent::AABBounderComponent(GameObject & gameObject, unsigned int weight, const AABox & box) :
    BounderComponent(gameObject, weight),
    m_box(box),
    m_transBox(m_box),
    m_prevTransBox(m_transBox)
{}

void AABBounderComponent::update(float dt) {
    m_transBox = m_spatial->orientation() == glm::quat() ?
        transformAABox(m_box, m_spatial->position(), m_spatial->scale()) :
        transformAABox(m_box, m_spatial->modelMatrix());

    m_isChange = m_spatial->isChange();
    m_prevTransBox = m_isChange ?
        m_spatial->prevOrientation() == glm::quat() ?
            transformAABox(m_box, m_spatial->prevPosition(), m_spatial->prevScale()) :
            transformAABox(m_box, m_spatial->prevModelMatrix()) :
        m_transBox;
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
    glm::vec3 center(this->center());
    float radius(glm::length(m_transBox.max - center));
    return Sphere(center, radius);
}

bool AABBounderComponent::isCritical() const {
    if (!m_isChange) {
        return false;
    }

    glm::vec3 delta(center() - prevCenter());
    glm::vec3 boxRadii((m_transBox.max - m_transBox.min) * 0.5f);
    return glm::abs(delta.x) > boxRadii.x || glm::abs(delta.y) > boxRadii.y || glm::abs(delta.z) > boxRadii.z;
}



Sphere SphereBounderComponent::transformSphere(const Sphere & sphere, const glm::mat4 & transMat, const glm::vec3 & scale) {
    return Sphere(
        glm::vec3(transMat * glm::vec4(sphere.origin, 1.0f)),
        glm::compMax(scale) * sphere.radius
    );
}

SphereBounderComponent::SphereBounderComponent(GameObject & gameObject, unsigned int weight, const Sphere & sphere) :
    BounderComponent(gameObject, weight),
    m_sphere(sphere),
    m_transSphere(m_sphere),
    m_prevTransSphere(m_transSphere)
{}

void SphereBounderComponent::update(float dt) {
    m_transSphere = transformSphere(m_sphere, m_spatial->modelMatrix(), m_spatial->scale());

    m_isChange = m_spatial->isChange();
    m_prevTransSphere = m_isChange ? transformSphere(m_sphere, m_spatial->prevModelMatrix(), m_spatial->prevScale()) : m_transSphere;
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

bool SphereBounderComponent::isCritical() const {
    if (!m_isChange) {
        return false;
    }

    return glm::length2(m_transSphere.origin - m_prevTransSphere.origin) > m_transSphere.radius * m_transSphere.radius;
}



Capsule CapsuleBounderComponent::transformCapsule(const Capsule & capsule, const glm::mat4 & transMat, const glm::vec3 & scale) {
    float transRadius(glm::max(scale.x, scale.z) * capsule.radius);
    return Capsule(
        glm::vec3(transMat * glm::vec4(capsule.center, 1.0f)),
        transRadius,
        glm::max(0.0f, scale.y * (capsule.height + 2.0f * capsule.radius) - 2.0f * transRadius)
    );
}

CapsuleBounderComponent::CapsuleBounderComponent(GameObject & gameObject, unsigned int weight, const Capsule & capsule) :
    BounderComponent(gameObject, weight),
    m_capsule(capsule),
    m_transCapsule(m_capsule),
    m_prevTransCapsule(m_transCapsule)
{}

void CapsuleBounderComponent::update(float dt) {
    m_transCapsule = transformCapsule(m_capsule, m_spatial->modelMatrix(), m_spatial->scale());

    m_isChange = m_spatial->isChange();
    m_prevTransCapsule = m_isChange ? transformCapsule(m_capsule, m_spatial->prevModelMatrix(), m_spatial->prevScale()) : m_transCapsule;
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

bool CapsuleBounderComponent::isCritical() const {
    if (!m_isChange) {
        return false;
    }

    glm::vec3 delta(m_transCapsule.center - m_prevTransCapsule.center);
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