#include "BounderShader.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Component/CollisionComponents/CollisionComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"

namespace {

glm::mat4 detAABBMat(const AABBounderComponent & bounder) {
    const AABox & span(bounder.transBox());
    glm::vec3 loc((span.max - span.min) * 0.5f + span.min);
    glm::vec3 scale(span.max - loc);
    return Util::compositeTransform(scale, loc);
}

glm::mat4 detSphereMat(const SphereBounderComponent & bounder) {
    const Sphere & sphere(bounder.transSphere());
    return Util::compositeTransform(glm::vec3(sphere.radius), sphere.origin);
}

std::pair<glm::mat4, glm::mat4> detCapsuleCapMats(const CapsuleBounderComponent & bounder) {
    const Capsule & capsule(bounder.transCapsule());
    glm::vec3 offset(0.0f, capsule.height * 0.5f, 0.0f);
    return {
        Util::compositeTransform(glm::vec3( capsule.radius), capsule.center + offset),
        Util::compositeTransform(glm::vec3(-capsule.radius), capsule.center - offset)
    };
}

glm::mat4 detCapsuleRodMat(const CapsuleBounderComponent & bounder) {
    const Capsule & capsule(bounder.transCapsule());
    return Util::compositeTransform(glm::vec3(capsule.radius, capsule.height * 0.5f, capsule.radius), capsule.center);
}

}



BounderShader::BounderShader(const std::string & vertFile, const std::string & fragFile, const CollisionSystem & collisionSys, const CameraComponent & cam) :
    Shader(vertFile, fragFile),
    m_collisionSystem(&collisionSys),
    m_camera(&cam),
    
    m_aabVBO(0), m_aabIBO(0), m_aabVAO(0),
    m_sphereVBO(0), m_sphereIBO(0), m_sphereVAO(0),
    m_capVBO(0), m_capIBO(0), m_capVAO(0),
    m_rodVBO(0), m_rodIBO(0), m_rodVAO(0),
    m_nAABIndices(0), m_nSphereIndices(0), m_nCapIndices(0), m_nRodIndices(0)
{}

bool BounderShader::init() {
    if (!Shader::init()) {
        return false;
    }
    /* Add attributes */
    addAttribute("in_pos");

    /* Add uniforms */
    addUniform("u_modelMat");
    addUniform("u_viewMat");
    addUniform("u_projMat");

    addUniform("u_color");

    if (!initAABMesh() || !initSphereMesh() || !initCapMesh() || !initRodMesh()) {
        std::cerr << "Failed to initialize collider shader meshes" << std::endl;
    }

    return true;
}

void BounderShader::render(const std::string & name, const std::vector<Component *> & components_) {
    ImGui::Begin("BoundingShader");
    ImGui::Selectable("Render", &isActive);
    ImGui::End();

    if (!isActive) {
        return;
    }
    loadMat4(getUniform("u_viewMat"), m_camera->getView());
    loadMat4(getUniform("u_projMat"), m_camera->getProj());

    for (auto & comp_ : m_collisionSystem->components()) {
        const BounderComponent & comp(*static_cast<const BounderComponent *>(comp_));

        loadVec3(getUniform("u_color"), comp.wasCollision() ? comp.wasAdjustment() ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f));

        if (dynamic_cast<const AABBounderComponent *>(&comp)) {
            const AABBounderComponent & aabbBounder(static_cast<const AABBounderComponent &>(comp));
            loadMat4(getUniform("u_modelMat"), detAABBMat(aabbBounder));

            glBindVertexArray(m_aabVAO);
            glDrawElements(GL_LINES, m_nAABIndices, GL_UNSIGNED_INT, nullptr);
        }
        else if (dynamic_cast<const SphereBounderComponent *>(&comp)) {
            const SphereBounderComponent & sphereBounder(static_cast<const SphereBounderComponent &>(comp));
            loadMat4(getUniform("u_modelMat"), detSphereMat(sphereBounder));

            glBindVertexArray(m_sphereVAO);
            glDrawElements(GL_LINES, m_nSphereIndices, GL_UNSIGNED_INT, nullptr);
        }
        else if (dynamic_cast<const CapsuleBounderComponent *>(&comp)) {
            const CapsuleBounderComponent & capsuleBounder(static_cast<const CapsuleBounderComponent &>(comp));

            auto capMats(detCapsuleCapMats(capsuleBounder));
            glm::mat4 & upperCapMat(capMats.first), & lowerCapMat(capMats.second);
            loadMat4(getUniform("u_modelMat"), upperCapMat);
            glBindVertexArray(m_capVAO);
            glDrawElements(GL_LINES, m_nCapIndices, GL_UNSIGNED_INT, nullptr);            
            loadMat4(getUniform("u_modelMat"), lowerCapMat);
            glDrawElements(GL_LINES, m_nCapIndices, GL_UNSIGNED_INT, nullptr);
            
            loadMat4(getUniform("u_modelMat"), detCapsuleRodMat(capsuleBounder));
            glBindVertexArray(m_rodVAO);
            glDrawElements(GL_LINES, m_nRodIndices, GL_UNSIGNED_INT, nullptr);
        }
    }

    glBindVertexArray(0);
}

bool BounderShader::initAABMesh() {
    int nVerts(8);
    glm::vec3 locs[8]{
        { -1.0f, -1.0f, -1.0f },
        {  1.0f, -1.0f, -1.0f },
        { -1.0f,  1.0f, -1.0f },
        {  1.0f,  1.0f, -1.0f },
        { -1.0f, -1.0f,  1.0f },
        {  1.0f, -1.0f,  1.0f },
        { -1.0f,  1.0f,  1.0f },
        {  1.0f,  1.0f,  1.0f }
    };

    unsigned int nIndices(24);
    unsigned int indices[24]{
        0, 4,   1, 5,   2, 6,   3, 7,
        0, 2,   1, 3,   4, 6,   5, 7,
        0, 1,   2, 3,   4, 5,   6, 7
    };
    
    glGenVertexArrays(1, &m_aabVAO);
    glBindVertexArray(m_aabVAO);

    glGenBuffers(1, &m_aabVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_aabVBO);
    glBufferData(GL_ARRAY_BUFFER, nVerts * sizeof(glm::vec3), locs, GL_STATIC_DRAW);

    glGenBuffers(1, &m_aabIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_aabIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_nAABIndices = nIndices;

    return glGetError() == GL_NO_ERROR;
}

bool BounderShader::initSphereMesh() {
    constexpr int arcLOD = 16;
    int nCircleVerts(arcLOD * 4);
    int nVerts(3 * nCircleVerts);
    std::unique_ptr<glm::vec3[]> locs(new glm::vec3[nVerts]);

    float dTheta(2.0f * glm::pi<float>() / nCircleVerts);
    for (int i(0); i < nCircleVerts; ++i) {
        float theta(i * dTheta);
        float x(std::cos(theta)), y(std::sin(theta));
        locs[0 * nCircleVerts + i] = glm::vec3(x, y, 0.0f);
        locs[1 * nCircleVerts + i] = glm::vec3(y, 0.0f, x);
        locs[2 * nCircleVerts + i] = glm::vec3(0.0f, x, y);
    }

    int nIndices(2 * nVerts);
    std::unique_ptr<int[]> indices(new int[nIndices]);
    for (int i(0); i < nCircleVerts; ++i) {
        indices[(0 * nCircleVerts + i) * 2 + 0] = 0 * int(nCircleVerts) + i + 0;
        indices[(0 * nCircleVerts + i) * 2 + 1] = 0 * int(nCircleVerts) + i + 1;
        indices[(1 * nCircleVerts + i) * 2 + 0] = 1 * int(nCircleVerts) + i + 0;
        indices[(1 * nCircleVerts + i) * 2 + 1] = 1 * int(nCircleVerts) + i + 1;
        indices[(2 * nCircleVerts + i) * 2 + 0] = 2 * int(nCircleVerts) + i + 0;
        indices[(2 * nCircleVerts + i) * 2 + 1] = 2 * int(nCircleVerts) + i + 1;
    }
    indices[nCircleVerts * 2 - 1] = int(0 * nCircleVerts);
    indices[nCircleVerts * 4 - 1] = int(1 * nCircleVerts);
    indices[nCircleVerts * 6 - 1] = int(2 * nCircleVerts);
    
    glGenVertexArrays(1, &m_sphereVAO);
    glBindVertexArray(m_sphereVAO);

    glGenBuffers(1, &m_sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, nVerts * sizeof(glm::vec3), locs.get(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_sphereIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphereIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned int), indices.get(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_nSphereIndices = nIndices;

    return glGetError() == GL_NO_ERROR;
}

bool BounderShader::initCapMesh() {
    constexpr int arcLOD = 16;
    int nCircleVerts(arcLOD * 4);
    int nVerts(2 * nCircleVerts + 2);
    std::unique_ptr<glm::vec3[]> locs(new glm::vec3[nVerts]);

    float dTheta(2.0f * glm::pi<float>() / nCircleVerts);
    int vi(0);
    for (int i(0); i < nCircleVerts; ++i) {
        float theta(i * dTheta);
        locs[vi++] = glm::vec3(std::cos(theta), 0.0f, -std::sin(theta));
    }
    for (int i(0); i < nCircleVerts / 2 + 1; ++i) {
        locs[vi++] = glm::vec3(-locs[i].x, -locs[i].z, 0.0f);
    }
    for (int i(0); i < nCircleVerts / 2 + 1; ++i) {
        locs[vi++] = glm::vec3(0.0f, -locs[i].z, -locs[i].x);
    }
    
    int nIndices(4 * nCircleVerts);
    std::unique_ptr<int[]> indices(new int[nIndices]);
    int ii(0); vi = 0;
    for (int i(0); i < nCircleVerts; ++i) {
        indices[ii++] = vi;
        indices[ii++] = vi + 1;
        ++vi;
    }
    indices[ii - 1] = 0;
    for (int i(0); i < nCircleVerts / 2; ++i) {
        indices[ii++] = vi;
        indices[ii++] = vi + 1;
        ++vi;
    }
    ++vi;
    for (int i(0); i < nCircleVerts / 2; ++i) {
        indices[ii++] = vi ;
        indices[ii++] = vi + 1;
        ++vi;
    }
    
    glGenVertexArrays(1, &m_capVAO);
    glBindVertexArray(m_capVAO);

    glGenBuffers(1, &m_capVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_capVBO);
    glBufferData(GL_ARRAY_BUFFER, nVerts * sizeof(glm::vec3), locs.get(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_capIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_capIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned int), indices.get(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_nCapIndices = nIndices;

    return glGetError() == GL_NO_ERROR;
}

bool BounderShader::initRodMesh() {
    constexpr int arcLOD = 16;
    int nCircleVerts(arcLOD * 4);
    int nVerts(nCircleVerts + 8);
    std::unique_ptr<glm::vec3[]> locs(new glm::vec3[nVerts]);

    float dTheta(2.0f * glm::pi<float>() / nCircleVerts);
    for (int i(0); i < nCircleVerts; ++i) {
        float theta(i * dTheta);
        locs[i] = glm::vec3(std::cos(theta), 0.0f, -std::sin(theta));
    }
    locs[nCircleVerts + 0] = glm::vec3( 1.0f, -1.0f,  0.0f);
    locs[nCircleVerts + 1] = glm::vec3( 1.0f,  1.0f,  0.0f);
    locs[nCircleVerts + 2] = glm::vec3( 0.0f, -1.0f,  1.0f);
    locs[nCircleVerts + 3] = glm::vec3( 0.0f,  1.0f,  1.0f);
    locs[nCircleVerts + 4] = glm::vec3(-1.0f, -1.0f,  0.0f);
    locs[nCircleVerts + 5] = glm::vec3(-1.0f,  1.0f,  0.0f);
    locs[nCircleVerts + 6] = glm::vec3( 0.0f, -1.0f, -1.0f);
    locs[nCircleVerts + 7] = glm::vec3( 0.0f,  1.0f, -1.0f);
    
    int nIndices(2 * nCircleVerts + 8);
    std::unique_ptr<int[]> indices(new int[nIndices]);
    for (int i(0); i < nCircleVerts; ++i) {
        indices[2 * i + 0] = i + 0;
        indices[2 * i + 1] = i + 1;
    }
    indices[2 * nCircleVerts - 1] = 0;
    for (int i(0); i < 8; ++i) {
        indices[2 * nCircleVerts + i] = int(nCircleVerts) + i;
    }
    
    glGenVertexArrays(1, &m_rodVAO);
    glBindVertexArray(m_rodVAO);

    glGenBuffers(1, &m_rodVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_rodVBO);
    glBufferData(GL_ARRAY_BUFFER, nVerts * sizeof(glm::vec3), locs.get(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_rodIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rodIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned int), indices.get(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_nRodIndices = nIndices;

    return glGetError() == GL_NO_ERROR;
}