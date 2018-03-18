#include "Orientable.hpp"

#include "Util/Util.hpp"



Orientable::Orientable() :
    m_orientation(),
    m_prevOrientation(m_orientation),
    m_orientMatrix(),
    m_prevOrientMatrix(m_orientMatrix),
    m_isChange(false)
{}

Orientable::Orientable(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w) :
    Orientable(glm::mat3(u, v, w))
{}

Orientable::Orientable(const glm::quat & orient) :
    m_orientation(orient),
    m_prevOrientation(m_orientation),
    m_orientMatrix(glm::toMat3(orient)),
    m_prevOrientMatrix(m_orientMatrix),
    m_isChange(false)
{}

Orientable::Orientable(const glm::mat3 & orient) :
    m_orientation(glm::toQuat(orient)),
    m_prevOrientation(m_orientation),
    m_orientMatrix(orient),
    m_prevOrientMatrix(m_orientMatrix),
    m_isChange(false)
{}

void Orientable::update() {
    m_prevOrientation = m_orientation;
    m_prevOrientMatrix = m_orientMatrix;
    m_isChange = false;
}

void Orientable::setOrientation(const glm::quat & orient) {
    m_orientation = orient;
    m_prevOrientation = m_orientation;
    m_orientMatrix = glm::toMat3(m_orientation);
    m_prevOrientMatrix = m_orientMatrix;
    m_isChange = false;
}

void Orientable::setOrientation(const glm::mat3 & orient) {
    m_orientMatrix = orient;
    m_prevOrientMatrix = m_orientMatrix;
    m_orientation = glm::toQuat(m_orientMatrix);
    m_prevOrientation = m_orientation;
    m_isChange = false;
}

void Orientable::rotate(const glm::quat & rot) {
    if (rot != glm::quat()) {
        m_orientation = rot * m_orientation;
        m_prevOrientMatrix = m_orientMatrix;
        m_orientMatrix = glm::toMat3(m_orientation);
        m_isChange = true;
    }
}

void Orientable::rotate(const glm::mat3 & rot) {
    if (rot != glm::mat3()) {
        m_orientMatrix = m_orientMatrix * rot;
        m_prevOrientation = m_orientation;
        m_orientation = glm::toQuat(m_orientMatrix);
        m_isChange = true;
    }
}

void Orientable::setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w) {
    m_orientMatrix[0] = u; m_orientMatrix[1] = v; m_orientMatrix[2] = w;;
    m_prevOrientMatrix = m_orientMatrix;
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