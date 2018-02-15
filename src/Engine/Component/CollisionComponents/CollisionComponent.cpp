#include "CollisionComponent.hpp"

#include <tuple>

#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/norm.hpp"

#include "Component/SpatialComponents/SpatialComponent.hpp"



BounderComponent::BounderComponent(int weight) :
    m_weight(weight),
    m_collisionFlag(false),
    m_adjustmentFlag(false)
{}


AABBounderComponent::AABBounderComponent(int weight, const AABox & box) :
    BounderComponent(weight),
    m_box(box),
    m_transBox(m_box)
{}

void AABBounderComponent::update(float dt) {
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

Sphere AABBounderComponent::enclosingSphere() const {
    glm::fvec3 center(0.5f * (m_transBox.max + m_transBox.min));
    float radius(glm::length(m_transBox.max - center));
    return Sphere(center, radius);
}



SphereBounderComponent::SphereBounderComponent(int weight, const Sphere & sphere) :
    BounderComponent(weight),
    m_sphere(sphere),
    m_transSphere(m_sphere)
{}

void SphereBounderComponent::update(float dt) {
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

Sphere SphereBounderComponent::enclosingSphere() const {
    return m_transSphere;
}



CapsuleBounderComponent::CapsuleBounderComponent(int weight, const Capsule & capsule) :
    BounderComponent(weight),
    m_capsule(capsule),
    m_transCapsule(m_capsule)
{}

void CapsuleBounderComponent::update(float dt) {
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

Sphere CapsuleBounderComponent::enclosingSphere() const {
    return Sphere(m_capsule.center, m_capsule.height * 0.5 + m_capsule.radius);
}



namespace {

std::pair<glm::vec3, glm::vec3> detMeshSpan(int nVerts, const glm::vec3 * positions) {    
    glm::vec3 min(Util::infinity), max(-Util::infinity);
    for (int i(0); i < nVerts; ++i) {
        min = glm::min(min, positions[i]);
        max = glm::max(max, positions[i]);
    }
    return { min, max };
}

float detMaxRadius(int n, const glm::vec3 * positions, const glm::vec3 & center) {
    float maxR2(0.0f);
    for (int i(0); i < n; ++i) {
        float r2(glm::length2(positions[i] - center));
        if (r2 > maxR2) maxR2 = r2;
    }
    return std::sqrt(maxR2);
}

// returns min radius, absolute y upper, and absolute y lower
std::tuple<float, float, float> detCapsuleSpecs(int n, const glm::vec3 * positions, const glm::vec3 & center) {
    float maxR2(0.0f);
    for (int i(0); i < n; ++i) {
        float r2(glm::length2(glm::vec2(positions[i].x - center.x, positions[i].z - center.z)));
        if (r2 > maxR2) maxR2 = r2;
    }
    float r(std::sqrt(maxR2));

    float maxQy(-Util::infinity), minQy(Util::infinity);
    for (int i(0); i < n; ++i) {
        float a(std::sqrt(maxR2 - glm::length2(glm::vec2(positions[i].x - center.x, positions[i].z - center.z))));
        if (positions[i].y >= center.y) {
            float qy(positions[i].y - a);
            if (qy > maxQy) maxQy = qy;
        }
        if (positions[i].y <= center.y) {
            float qy(positions[i].y + a);
            if (qy < minQy) minQy = qy;
        }
    }

    if (maxQy < minQy) {
        maxQy = minQy = (maxQy + minQy) * 0.5f;
    }

    return { r, maxQy, minQy };
}

}

std::unique_ptr<BounderComponent> createBounderFromMesh(int weight, const Mesh & mesh, bool allowAAB, bool allowSphere, bool allowCapsule) {
    if (!allowAAB && !allowSphere && !allowCapsule) {
        allowAAB = allowSphere = allowCapsule = true;
    }

    AABox box; Sphere sphere; Capsule capsule;
    float boxV(Util::infinity), sphereV(Util::infinity), capsuleV(Util::infinity);

    int nVerts(int(mesh.buffers.vertBuf.size()) / 3);
    const glm::vec3 * positions(reinterpret_cast<const glm::vec3 *>(mesh.buffers.vertBuf.data()));
    auto span(detMeshSpan(nVerts, positions));
    glm::vec3 & spanMin(span.first), & spanMax(span.second);
    glm::vec3 center((spanMax - spanMin) * 0.5f + spanMin);

    if (allowAAB) {
        box = AABox(spanMin, spanMax);
        boxV = box.volume();
    }

    if (allowSphere) {
        float radius(detMaxRadius(nVerts, positions, center));
        sphere = Sphere(center, radius);
        sphereV = sphere.volume();
    }

    if (allowCapsule) {
        float minRad, yUpper, yLower;
        std::tie(minRad, yUpper, yLower) = detCapsuleSpecs(nVerts, positions, center);
        float capsuleHeight(yUpper - yLower);
        glm::vec3 capsuleCenter(center.x, yLower + capsuleHeight * 0.5f, center.z);
        capsule = Capsule(capsuleCenter, minRad, capsuleHeight);
        capsuleV = capsule.volume();
    }

    if (allowAAB && boxV <= sphereV && boxV <= capsuleV) {
        return std::unique_ptr<BounderComponent>(new AABBounderComponent(weight, box));
    }
    else if (allowSphere && sphereV <= boxV && sphereV <= capsuleV) {
        return std::unique_ptr<BounderComponent>(new SphereBounderComponent(weight, sphere));
    }
    else if (allowCapsule && capsuleV <= boxV && capsuleV <= sphereV) {
        return std::unique_ptr<BounderComponent>(new CapsuleBounderComponent(weight, capsule));
    }

    return nullptr;
}