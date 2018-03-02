#include "Scaleable.hpp"



Scaleable::Scaleable() :
    m_scale(1.0f),
    m_prevScale(m_scale),
    m_isChange(false)
{}

Scaleable::Scaleable(const glm::vec3 & scale) :
    m_scale(scale),
    m_prevScale(m_scale),
    m_isChange(false)
{}

void Scaleable::update() {
    m_prevScale = m_scale;
    m_isChange = false;
}

void Scaleable::setScale(const glm::vec3 & scale) {
    m_scale = scale;
    m_prevScale = m_scale;
    m_isChange = false;
}

void Scaleable::scale(const glm::vec3 & factor) {
    m_prevScale = m_scale;
    if (factor == glm::vec3(1.0f)) {
        m_isChange = false;
    }
    else {
        m_scale *= factor;
        m_isChange = true;
    }
}

glm::vec3 Scaleable::scale(float interpP) const {
    return m_isChange ? glm::mix(m_prevScale, m_scale, interpP) : m_scale;
}