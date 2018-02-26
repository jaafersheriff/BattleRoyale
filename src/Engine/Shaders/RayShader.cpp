#include "RayShader.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"



RayShader::RayShader(const String & vertFile, const String & fragFile) :
    Shader(vertFile, fragFile),    
    m_positions(),
    m_vbo(0),
    m_ibo(0),
    m_vao(0),
    m_positionsValid(false)
{}

bool RayShader::init() {
    if (!Shader::init()) {
        return false;
    }

    setEnabled(false);

    /* Add attributes */
    addAttribute("in_pos");
    /* Add uniforms */
    addUniform("u_viewMat");
    addUniform("u_projMat");
    addUniform("u_invLength");
    
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return glGetError() == GL_NO_ERROR;
}

void RayShader::render(const CameraComponent * camera, const Vector<Component *> & components_) {
    if (!camera) {
        return;
    }
    if (m_positions.size() < 2) {
        return;
    }

    if (!m_positionsValid) {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        loadFloat(getUniform("u_invLength"), 1.0f / (m_positions.size() - 1));
        m_positionsValid = true;
    }

    loadMat4(getUniform("u_viewMat"), camera->getView());
    loadMat4(getUniform("u_projMat"), camera->getProj());

    glBindVertexArray(m_vao);
    glDrawArrays(GL_LINE_STRIP, 0, m_positions.size());
    glBindVertexArray(0);
}

void RayShader::setPositions(const Vector<glm::vec3> & positions) {
    m_positions = positions;
    m_positionsValid = false;
}