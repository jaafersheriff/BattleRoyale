#pragma once
#ifndef _PARTICLE_HPP_
#define _PARTICLE_HPP_

#include "glm/glm.hpp"
class Particle {
    public:
        glm::vec3 m_position;
        int m_type;
        int m_i;
        int m_total;

        Particle(int type, int i, int total, glm::vec3 origin);

        void update(float tData);
        void updateMovement(float tData);
        glm::vec3 sphereMove(float tData);
        glm::vec3 sphereExplode(float tData);
};

#endif // !_PARTICLE_HPP_