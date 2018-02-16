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

        void update(float tData);
        void updateMovement(float tData);
        glm::vec3 sphereMove(float tData);
        glm::vec3 sphereExplode(float tData);
};

#endif // !_PARTICLE_HPP_