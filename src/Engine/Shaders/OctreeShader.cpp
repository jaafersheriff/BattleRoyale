#include "OctreeShader.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"
#include "System/CollisionSystem.hpp"

namespace {

glm::mat4 detAABBMat(const AABox & box) {
    glm::vec3 loc((box.max - box.min) * 0.5f + box.min);
    glm::vec3 scale(box.max - loc);
    return Util::compositeTransform(scale, loc);
}

}



OctreeShader::OctreeShader(const String & vertFile, const String & fragFile) :
    Shader(vertFile, fragFile),
    
    m_aabVBO(0), m_aabIBO(0), m_aabVAO(0),
    m_nAABIndices(0)
{}

bool OctreeShader::init() {
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

    if (!initAABMesh()) {
        std::cerr << "Failed to initialize octree AAB mesh" << std::endl;
    }

    return true;
}

void OctreeShader::render(const CameraComponent * camera, const Vector<Component *> & components_) {
    if (!camera) {
        return;
    }
    if (!CollisionSystem::s_octree) {
        return;
    }

    glBindVertexArray(m_aabVAO);

    loadMat4(getUniform("u_viewMat"), camera->getView());
    loadMat4(getUniform("u_projMat"), camera->getProj());

    renderNode(*CollisionSystem::s_octree->m_root, 0, CollisionSystem::s_octree->m_rootRadius);

    glBindVertexArray(0);
}

void OctreeShader::renderNode(const Octree<BounderComponent *>::Node & node, int depth, float radius) {
    loadMat4(getUniform("u_modelMat"), detAABBMat(AABox(node.center - radius, node.center + radius)));

    int maxDepth(CollisionSystem::s_octree->m_maxDepth);
    float d(maxDepth == 0 ? 0.0f : float(depth) / float(maxDepth));
    loadVec3(getUniform("u_color"), glm::mix(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f), d));
    glDrawElements(GL_LINES, m_nAABIndices, GL_UNSIGNED_INT, nullptr);

    if (node.nodes) {
        for (int o(0), op(1); o < 8; ++o, op <<= 1) {
            if (node.active & op) {
                renderNode(node.nodes[o], depth + 1, radius * 0.5f);
            }
        }
    }
}

bool OctreeShader::initAABMesh() {
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