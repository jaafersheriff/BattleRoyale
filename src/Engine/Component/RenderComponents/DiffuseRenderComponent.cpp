#include "DiffuseRenderComponent.hpp"

#include "glm/gtx/norm.hpp"
#include "glm/gtx/component_wise.hpp"

#include "Model/Mesh.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Util/Util.hpp"



namespace {

std::pair<glm::vec3, glm::vec3> detMeshSpan(int nVerts, const glm::vec3 * positions) {    
    glm::vec3 min(Util::infinity()), max(-Util::infinity());
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

}



Sphere DiffuseRenderComponent::detEnclosingSphere(const Mesh & mesh) {
    int nVerts(int(mesh.buffers.vertBuf.size()) / 3);
    const glm::vec3 * positions(reinterpret_cast<const glm::vec3 *>(mesh.buffers.vertBuf.data()));
    auto span(detMeshSpan(nVerts, positions));
    glm::vec3 & spanMin(span.first), & spanMax(span.second);
    glm::vec3 center((spanMax - spanMin) * 0.5f + spanMin);
    float radius(detMaxRadius(nVerts, positions, center));
    return Sphere(center, radius);
}

DiffuseRenderComponent::DiffuseRenderComponent(
    GameObject & gameObject, const SpatialComponent & spatial,
    const Mesh & mesh, const ModelTexture & texture,
    bool toon, const glm::vec2 & tiling
) :
    Component(gameObject),
    m_mesh(mesh),
    m_modelTexture(texture),
    m_isToon(toon),
    m_tiling(tiling),
    m_spatial(&spatial),
    m_sphere(detEnclosingSphere(mesh)),
    m_transSphere(m_sphere)
{}

void DiffuseRenderComponent::update(float dt) {    
    m_transSphere.origin = glm::vec3(m_spatial->modelMatrix() * glm::vec4(m_sphere.origin, 1.0f));
    m_transSphere.radius = glm::compMax(m_spatial->scale()) * m_sphere.radius;
}