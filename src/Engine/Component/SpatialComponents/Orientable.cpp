#include "Orientable.hpp"

#include "Util/Util.hpp"



Orientable::Orientable() :
    m_u(1.0f, 0.0f, 0.0f),
    m_v(0.0f, 1.0f, 0.0f),
    m_w(0.0f, 0.0f, 1.0f),
    m_orientation(),
    m_prevOrientation(),
    m_orientMatrix(),
    m_isChange(false)
{}

Orientable::Orientable(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w) :
    m_u(u),
    m_v(v),
    m_w(w),
    m_orientation(),
    m_prevOrientation(),
    m_orientMatrix(),
    m_isChange(false)
{
    detOrientMatrixFromUVW();
    m_orientation = glm::toQuat(m_orientMatrix);
    m_prevOrientation = m_orientation;
}

void Orientable::update() {
    m_prevOrientation = m_orientation;
    m_isChange = false;
}

void Orientable::setOrientation(const glm::quat & orient) {
    m_orientation = orient;
    m_prevOrientation = m_orientation;
    m_orientMatrix = glm::toMat3(m_orientation);
    detUVWFromOrientMatrix();
    m_isChange = false;
}

void Orientable::setOrientation(const glm::mat3 & orient) {
    m_orientMatrix = orient;
    m_orientation = glm::toQuat(m_orientMatrix);
    m_prevOrientation = m_orientation;
    detUVWFromOrientMatrix();
    m_isChange = false;
}

void Orientable::rotate(const glm::quat & rot) {
    m_prevOrientation = m_orientation;
    m_orientation = rot * m_orientation;
    m_orientMatrix = glm::toMat3(m_orientation);
    detUVWFromOrientMatrix();
    m_isChange = m_orientation != m_prevOrientation;
}

void Orientable::rotate(const glm::mat3 & rot) {
    m_prevOrientation = m_orientation;
    m_orientMatrix = m_orientMatrix * rot;
    m_orientation = glm::toQuat(m_orientMatrix);
    detUVWFromOrientMatrix();
    m_isChange = m_orientation != m_prevOrientation;
}

void Orientable::setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w) {
    m_u = u;
    m_v = v;
    m_w = w;
    detOrientMatrixFromUVW();
    m_orientation = glm::toQuat(m_orientMatrix);
    m_prevOrientation = m_orientation;
    m_isChange = false;
}

glm::quat Orientable::orientation(float interpP) const {
    // TODO: make sure lerp is good enough for our purposes. Could use
    // slerp, but it uses 4 trig functions, which is hella expensive
    return m_isChange ? glm::lerp(m_prevOrientation, m_orientation, interpP) : m_orientation;
}

glm::mat3 Orientable::orientMatrix(float interpP) const {
    return m_isChange ? glm::toMat3(orientation(interpP)) : m_orientMatrix;
}

void Orientable::detOrientMatrixFromUVW() {
    m_orientMatrix = Util::mapFrom(m_u, m_v, m_w);
}

void Orientable::detUVWFromOrientMatrix() {
    glm::mat3 trans(glm::transpose(m_orientMatrix));
    m_u = trans[0];
    m_v = trans[1];
    m_w = trans[2];
}