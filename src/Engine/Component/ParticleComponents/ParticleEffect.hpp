#pragma once
#ifndef _PARTICLEEFFECT_HPP_
#define _PARTICLEEFFECT_HPP_

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/random.hpp"
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
            bool active = false;
            float life = 0.0f;
            glm::vec3 position;
            glm::vec3 velocity;
            int meshID;
            int modelTextureID;
        }Particle;

        typedef struct {
            ParticleEffect::Type type;
            int n = 0; //Can be how many to spawn or, if rate is enable, the limit of particle count
            float effectDuration = 0.0f;
            float particleDuration = 0.0f;
            float variance = 0.0f;
            float rate = 0.0f; // rate at which particles spawn, 0 if all at once
            float angle = 2 * glm::pi<float>(); // default to 360 degrees
            float maxDist = 0.0f; //not 0, if applicable
            bool loop = false;
            float magnitude = 1.0f;
            Vector<glm::vec3>* accelerators;
            Vector<Mesh *>* meshes;
            Vector<ModelTexture *>* textures;
        }EffectParams;

        

    public:
        ParticleEffect();
        ParticleEffect(EffectParams *ep, const glm::vec3 & origin);
        ParticleEffect(EffectParams *ep, const glm::vec3 & origin, const glm::vec3 & direction);
        ParticleEffect(EffectParams *ep, const glm::vec3 & origin, const glm::vec3 & direction, const glm::vec3 & velocity);
   
    private:
        EffectParams *m_effectParams;
        glm::vec3 m_origin;
        glm::vec3 m_direction;
        glm::vec3 m_velocity;
        Vector<Particle*> m_particles;
        float m_life;
        Vector<glm::vec3> *m_activeParticlePositions;
        Vector<int> m_activeMap;

    public:
        void update(float dt);
        Mesh* getMesh(int i);
        ModelTexture* getModelTexture(int i);
        
        Vector<glm::vec3> * ActiveParticlePositions() {return m_activeParticlePositions; }
        glm::vec3   Origin()    { return m_origin; }
        int         Count()     { return m_effectParams->n; }
        float       Life()      { return m_life; }
        float       Duration()  { return m_effectParams->effectDuration; }
        static EffectParams* createEffectParams(
            ParticleEffect::Type type,
            int n,
            float effectDuration,
            float particleDuration,
            float variance,
            float rate,
            float angle,
            float maxDist,
            bool loop,
            float magnitude,
            Vector<glm::vec3>* accelerators,
            Vector<Mesh *>* meshes,
            Vector<ModelTexture *>* textures
            );

    private:
        void initParticle(Particle *p, int i, int meshID, int modelTextureID);
        void initPosition(Particle *p);
        void initVelocity(Particle *p);
        int meshSelect(ParticleEffect::Effect effect, int i);
        int modelTextureSelect(ParticleEffect::Effect effect, int i);
        Vector<ParticleEffect::Particle*> generateParticles(EffectParams *ep);
        Vector<glm::vec3> * getActiveParticlePositions();
        Vector<int> getActiveMap();
        void updatePosition(Particle* p, float dt);
        void updateActiveParticles(float dt);
        void sphereMotion(Particle* p, float speed);

};

#endif 
