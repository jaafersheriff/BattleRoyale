#include "Positionable.hpp"



Positionable::Positionable() :
    m_position(),
    m_prevPosition(m_position),
    m_isChange(false)
{}

Positionable::Positionable(const glm::vec3 & position) :
    m_position(position),
    m_prevPosition(m_position),
    m_isChange(false)
{}

void Positionable::update() {
    m_prevPosition = m_position;
    m_isChange = false;
}

void Positionable::setPosition(const glm::vec3 & position) {
    m_position = position;
    m_prevPosition = m_position;
    m_isChange = false;
}

void Positionable::move(const glm::vec3 & delta) {
    m_prevPosition = m_position;
    if (delta == glm::vec3()) {
        m_isChange = false;
    }
    else {
        m_position += delta;
        m_isChange = true;
    }
}

glm::vec3 Positionable::position(float interpP) const {
    return m_isChange ? glm::mix(m_prevPosition, m_position, interpP) : m_position;
}