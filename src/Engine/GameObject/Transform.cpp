#include "Transform.hpp"

#include "glm/gtc/matrix_transform.hpp"



Transform::Transform() :
    m_position(),
    m_scale(1.0f),
    m_rotation(),
    m_modelMatrix(),
    m_normalMatrix(),
    m_modelMatrixValid(false),
    m_normalMatrixValid(false)
{}

Transform::Transform(const glm::vec3 & loc, const glm::vec3 & scale, const glm::mat3 & rot) :
    m_position(loc),
    m_scale(scale),
    m_rotation(rot),
    m_modelMatrix(),
    m_normalMatrix(),
    m_modelMatrixValid(false),
    m_normalMatrixValid(false)
{
    detModelMatrix();
    detNormalMatrix();
}

void Transform::setPosition(const glm::vec3 & loc) {
    m_position = loc;
    m_modelMatrixValid = false;
}

void Transform::move(const glm::vec3 & delta) {
    m_position += delta;
    m_modelMatrixValid = false;
}

void Transform::setScale(const glm::vec3 & scale) {
    m_scale = scale;
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
}

void Transform::scale(const glm::vec3 & factor) {
    m_scale *= factor;
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
}

void Transform::setRotation(const glm::mat3 & rot) {
    m_rotation = rot;
    m_normalMatrixValid = false;
}

void Transform::rotate(const glm::mat3 & mat) {
    m_rotation = mat * m_rotation;
    m_normalMatrixValid = false;
}
    
const glm::mat4 & Transform::modelMatrix() const {
    if (!m_modelMatrixValid) detModelMatrix();
    return m_modelMatrix;
}

const glm::mat3 & Transform::normalMatrix() const {
    if (!m_normalMatrixValid) detNormalMatrix();
    return m_normalMatrix;
}

void Transform::detModelMatrix() const {
    m_modelMatrix = glm::translate(glm::mat4(m_rotation) * glm::scale(glm::mat4(), m_scale), m_position);
    m_modelMatrixValid = true;
}

void Transform::detNormalMatrix() const {
    // this is valid and waaaaaaaay faster than inverting the model matrix
    m_normalMatrix = m_rotation * glm::mat3(glm::scale(glm::mat4(), 1.0f / m_scale));
    m_normalMatrixValid = true;
}