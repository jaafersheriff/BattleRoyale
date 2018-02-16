#include "Particle.hpp"
#include <glm/gtc/constants.hpp>

Particle::Particle(int type, int i, int total, glm::vec3 origin) :
    position(origin),
    type(type),
    i(i),
    total(total)
{
}

void Particle::update(float tData) {
 
    updateMovement(tData);
}

void Particle::updateMovement(float tData) {
    if (type == 0) {
        position = sphereExplode(tData);
    }
}

glm::vec3 Particle::sphereMove(float tData) {
    float phi = glm::golden_ratio<float>();

    float z = 1 - (2 * float(i) / float(total - 1));
    float radius = sqrt(1 - z * z);
    float theta = 2 * glm::pi<float>() * (2 - phi) * float(i);
    float default_speed = 10.f;

    return position + float(default_speed * tData) * glm::vec3(radius * cos(theta), radius * sin(theta), z);
}

glm::vec3 Particle::sphereExplode(float tData) {
    float phi = glm::golden_ratio<float>();

    float z = 1 - (2 * float(i) / float(total - 1));
    float radius = sqrt(1 - z * z);
    float theta = 2 * glm::pi<float>() * (2 - phi) * float(i);
    float speed = 50.f;
    float accel = -100.0f;
    return position + float(speed * tData + accel * tData * tData) * glm::vec3(radius * cos(theta), radius * sin(theta), z);
}

