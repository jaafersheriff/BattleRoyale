#include "CollisionComponent.hpp"

#include "glm/gtx/component_wise.hpp"

#include "Component/SpatialComponents/SpatialComponent.hpp"



BounderComponent::BounderComponent(int weight) :
    m_weight(weight),
    m_isCollision(false)
{}



AABBounderComponent::AABBounderComponent(int weight, const AABox & box) :
    BounderComponent(weight),
    m_box(box),
    m_transBox(m_box)
{}

void AABBounderComponent::update() {
    if (!gameObject->getSpatial()) {
        return;
    }

    SpatialComponent & spat(*gameObject->getSpatial());

    // no rotation
    if (spat.rotation() == glm::mat3()) {
        m_transBox.min = m_box.min * spat.scale() + spat.position();
        m_transBox.max = m_box.max * spat.scale() + spat.position();
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
        const glm::mat4 & modelMat(spat.modelMatrix());
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



SphereBounderComponent::SphereBounderComponent(int weight, const Sphere & sphere) :
    BounderComponent(weight),
    m_sphere(sphere),
    m_transSphere(m_sphere)
{}

void SphereBounderComponent::update() {
    if (!gameObject->getSpatial()) {
        return;
    }

    SpatialComponent & spat(*gameObject->getSpatial());

    m_transSphere.origin = spat.modelMatrix() * glm::vec4(m_sphere.origin, 1.0f);
    m_transSphere.radius = glm::compMax(spat.scale()) * m_sphere.radius;
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



CapsuleBounderComponent::CapsuleBounderComponent(int weight, const Capsule & capsule) :
    BounderComponent(weight),
    m_capsule(capsule),
    m_transCapsule(m_capsule)
{}

void CapsuleBounderComponent::update() {
    if (!gameObject->getSpatial()) {
        return;
    }

    SpatialComponent & spat(*gameObject->getSpatial());

    m_transCapsule.center = spat.modelMatrix() * glm::vec4(m_capsule.center, 1.0f);
    const glm::vec3 & scale(spat.scale());
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



std::unique_ptr<BounderComponent> createBounderFromMesh(const Mesh & mesh) {
    return std::move(createBounderFromMesh(mesh, true, true, true));
}

std::unique_ptr<BounderComponent> createBounderFromMesh(const Mesh & mesh, bool allowAAB, bool allowSphere, bool allowCapsule) {
    fspan3 span(entity.mesh().detSpan());
    AABox box(span);
    float boxV(box.volume());

    glm::vec3 center((span.max - span.min) * 0.5f + span.min);
    float radius(detMaxRadius(entity.mesh().nVerts, entity.mesh().getLocations(), center));
    Sphere sphere(center, radius);
    float sphereV(sphere.volume());

    glm::vec3 capsuleSpecs(detCapsuleSpecs(entity.mesh().nVerts, entity.mesh().getLocations(), center));
    float capsuleHeight(capsuleSpecs.y - capsuleSpecs.z);
    glm::vec3 capsuleCenter(center.x, center.y, capsuleSpecs.z + capsuleHeight * 0.5f);
    Capsule capsule(capsuleCenter, capsuleSpecs.x, capsuleHeight);
    float capsuleV(capsule.volume());

    BounderComponent * bounder;
    if (boxV <= sphereV) {
        if (boxV <= capsuleV) {
            bounder = qcu::Depot<BounderComponent>::add(new AABounderComponent(entity, weight, box));
        }
        else {
            bounder = qcu::Depot<BounderComponent>::add(new CapsuleBounderComponent(entity, weight, capsule));
        }
    }
    else {
        if (sphereV <= capsuleV) {
            bounder = qcu::Depot<BounderComponent>::add(new SphereBounderComponent(entity, weight, sphere));
        }
        else {
            bounder = qcu::Depot<BounderComponent>::add(new CapsuleBounderComponent(entity, weight, capsule));
        }
    }

    f_bounders.push_back(bounder);
    f_entityBounderComponents[&entity].push_back(bounder);
    f_potentials.insert(bounder);
    BounderComponentRenderer::get().add(*bounder);
    bounder->update();
}