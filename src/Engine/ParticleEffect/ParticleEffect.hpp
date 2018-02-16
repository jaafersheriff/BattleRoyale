#pragma once
#ifndef _PARTICLEEFFECT_HPP_
#define _PARTICLEEFFECT_HPP_

#include "glm/glm.hpp"
#include "Particle.hpp"
#include "Model/Mesh.hpp"
#include "Model/ModelTexture.hpp"
#include <vector>

class ParticleEffect {
    public:

        /* Enum for particle effect */
        enum ParticleEffectTypes {
            SPHERE_LINEAR
        };
        
        /* Attributes*/
        int type;
        int total;
        float duration;
        glm::vec3 origin;
        float life;

        std::vector<Particle> particles;
        float tData;

        /* Constructors */
        ParticleEffect(int type, int n, float duration, glm::vec3 origin);

        std::vector<Particle> generateParticles();

        void update(float dt);
};

#endif // !_PARTICLEEFFECT_HPP_
