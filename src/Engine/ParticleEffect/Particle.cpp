#include "Particle.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>

Particle::Particle(int type, int i, int total, glm::vec3 origin) :
    position(origin),
    type(type),
    i(i),
    total(total),
    origin(origin)
{
}

void Particle::update(double tData) {
 
    updateMovement(tData);
}

void Particle::die() {
    // Just Die
}

void Particle::updateMovement(double tData) {
    if (type == 0) {
        position = sphereExplode(tData);
    }
}

glm::vec3 Particle::sphereMove(double tData) {
    double phi = glm::golden_ratio<float>();

    float z = 1 - (2 * float(i) / float(total - 1));
    float radius = sqrt(1 - z * z);
    float theta = 2 * glm::pi<float>() * (2 - phi) * float(i);
    float default_speed = 10.f;

    return origin + float(default_speed * tData) * glm::vec3(radius * cos(theta), radius * sin(theta), z);
}

glm::vec3 Particle::sphereExplode(double tData) {
    double phi = glm::golden_ratio<float>();

    float z = 1 - (2 * float(i) / float(total - 1));
    float radius = sqrt(1 - z * z);
    float theta = 2 * glm::pi<float>() * (2 - phi) * float(i);
    float speed = 50.f;
    float accel = -100.0f;
    return origin + float(speed * tData + accel * tData * tData) * glm::vec3(radius * cos(theta), radius * sin(theta), z);
}

