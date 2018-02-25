#include "RayShader.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"



RayShader::RayShader(const String & vertFile, const String & fragFile) :
    Shader(vertFile, fragFile),    
    m_ray(),
    m_vbo(0),
    m_ibo(0),
    m_vao(0),
    m_rayValid(false)
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
    
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);

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
    if (m_ray.dir == glm::vec3()) {
        return;
    }

    if (!m_rayValid) {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glm::vec3 data[2]{ m_ray.pos, m_ray.pos + m_ray.dir };
        glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(glm::vec3), data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        m_rayValid = true;
    }

    loadMat4(getUniform("u_viewMat"), camera->getView());
    loadMat4(getUniform("u_projMat"), camera->getProj());

    glBindVertexArray(m_vao);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void RayShader::setRay(const Ray & ray) {
    m_ray = ray;
    m_rayValid = false;
}