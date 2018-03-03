#pragma once
#ifndef _PARTICLEEFFECT_HPP_
#define _PARTICLEEFFECT_HPP_

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "Particle.hpp"
#include "Model/Mesh.hpp"
#include "Model/ModelTexture.hpp"

class ParticleEffect {
    public:
        /* Enum for particle effect */
        enum Type {
            SPHERE,
            CONE,
            DISK,
        };

        enum Effect {
            BLOOD_SPLAT,
            EXPLOADING_GUTS,
            SIRACHA_FLAMETHROWER,
            SPICE_GRENADE,
            MUSTARD_SPLAT
        };

        typedef struct {
            ParticleEffect::Type type;
            int n = 0;
            float duration = 0.0f;
            float variance = 0.0f;
            float rate = 0.0f; // not 0, if applicable
            float angle = 2 * glm::pi<float>(); // default to 360 degrees
            float maxDist = 0.0f; //not 0, if applicable
            float mass = 1.0f;
            bool loop = false;
            glm::vec3* initVelocity = new glm::vec3(0);
            glm::vec3* forward = new glm::vec3(0);
            Vector<glm::vec3>* forceFactors;
            Vector<Mesh *>* meshes;
            Vector<ModelTexture *>* textures;
        }EffectParams;

        

    public:
        ParticleEffect(EffectParams *ep, const glm::vec3 & origin);
   
    private:
        EffectParams *m_ep;
        glm::vec3 m_origin;
        glm::vec3 m_euler;
        glm::vec3 m_scale;
        Vector<Particle> & m_particles;
        float m_life;

    public:
        void update(float dt);
        Mesh* getMesh(int i);
        ModelTexture* getModelTexture(int i);
        glm::vec3 Origin() { return m_origin; }
        glm::vec3 Euler() { return m_euler; }
        glm::vec3 Scale() { return m_scale; }
        static EffectParams* createEffectParams(
            ParticleEffect::Type type,
            int n,
            float duration,
            float variance,
            float rate,
            float angle,
            float maxDist,
            float mass,
            bool loop,
            glm::vec3* initVelocity,
            glm::vec3* forward,
            Vector<glm::vec3>* forceFactors,
            Vector<Mesh *>* meshes,
            Vector<ModelTexture *>* textures
            );

    private:
        Vector<Particle> generateParticles();

        
};

#endif 
