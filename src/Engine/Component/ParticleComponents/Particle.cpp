#include "Particle.hpp"
#include <glm/gtc/constants.hpp>

Particle::Particle(int type, int i, int total, glm::vec3 origin) :
    m_position(origin),
    m_type(type),
    m_i(i),
    m_total(total)
{
}

void Particle::update(float tData) {
 
    updateMovement(tData);
}

void Particle::updateMovement(float tData) {
    if (m_type == 0) {
        m_position = sphereExplode(tData);
    }
}

glm::vec3 Particle::sphereMove(float tData) {
    float phi = glm::golden_ratio<float>();

    float z = 1 - (2 * float(m_i) / float(m_total - 1));
    float radius = sqrt(1 - z * z);
    float theta = 2 * glm::pi<float>() * (2 - phi) * float(m_i);
    float default_speed = 10.f;

    return m_position + float(default_speed * tData) * glm::vec3(radius * cos(theta), radius * sin(theta), z);
}

glm::vec3 Particle::sphereExplode(float tData) {
    float phi = glm::golden_ratio<float>();

    float z = 1 - (2 * float(m_i) / float(m_total - 1));
    float radius = sqrt(1 - z * z);
    float theta = 2 * glm::pi<float>() * (2 - phi) * float(m_i);
    float speed = 50.f;
    float accel = -100.0f;
    return m_position + float(speed * tData + accel * tData * tData) * glm::vec3(radius * cos(theta), radius * sin(theta), z);
}

