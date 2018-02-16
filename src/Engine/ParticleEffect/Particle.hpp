#pragma once
#ifndef _PARTICLE_HPP_
#define _PARTICLE_HPP_

#include "glm/glm.hpp"
class Particle {
    public:
        /* Attributes*/
        glm::vec3 position;
        int type;
        int i;
        int total;
        glm::vec3 origin;

        /* Constructors */
        Particle(int type, int i, int total, glm::vec3 origin);

        void Particle::update(double tData);
        void Particle::updateMovement(double tData);
        glm::vec3 Particle::sphereMove(double tData);
        glm::vec3 Particle::sphereExplode(double tData);
};

#endif // !_PARTICLE_HPP_