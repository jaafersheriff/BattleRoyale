#pragma once
#ifndef _PARTICLEEFFECT_HPP_
#define _PARTICLEEFFECT_HPP_

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
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
            int i;
            glm::vec3 position;
            int meshID;
            int modelTextureID;
        }Particle;

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
            Vector<glm::vec3>* forceFactors;
            Vector<Mesh *>* meshes;
            Vector<ModelTexture *>* textures;
        }EffectParams;

        

    public:
        ParticleEffect();
        ParticleEffect(EffectParams *ep, const glm::vec3 & origin);
        ParticleEffect(EffectParams *ep, const glm::vec3 & origin, const glm::vec3 & direction);
   
    private:
        EffectParams *m_ep;
        glm::vec3 m_origin;
        glm::vec3 m_direction;
        Vector<Particle*> & m_particles;
        float m_life;

    public:
        void update(float dt);
        Mesh* getMesh(int i);
        ModelTexture* getModelTexture(int i);
        glm::vec3 Origin() { return m_origin; }
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
            Vector<glm::vec3>* forceFactors,
            Vector<Mesh *>* meshes,
            Vector<ModelTexture *>* textures
            );

    private:
        void ParticleEffect::pinit(ParticleEffect::Particle *p, int i, glm::vec3 pos, int meshID, int modelTextureID);
        int ParticleEffect::meshSelect(ParticleEffect::Effect effect, int i);
        int ParticleEffect::modelTextureSelect(ParticleEffect::Effect effect, int i);
        Vector<ParticleEffect::Particle*> generateParticles();
        void ParticleEffect::sphereMove(ParticleEffect::Particle* p, float dt);

};

#endif 
