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
        enum Type {
            SPHERE, 
            CONE, // NOT IMPLEMENTED
            DISK, //NOT IMPLEMENTED
        };

        enum Effect {
            BLOOD_SPLAT,
            EXPLOADING_GUTS, // NOT IMPLEMENTED
            SIRACHA_FLAMETHROWER, // NOT IMPLEMENTED
            SPICE_GRENADE, // NOT IMPLEMENTED
            MUSTARD_SPLAT //NOT IMPLEMENTED
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
            int randomOrientations = 0; //How many different random orientations can the particle have, 0 means just base.
            float variance = 0.0f; //randomness factor
            float rate = 0.0f; // rate at which particles spawn (particles/sec), 0 if all spawn at once
            float angle = 2 * glm::pi<float>(); // default to 360 degrees
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
        float m_nextActivation;

    public:
        void update(float dt);
        Mesh* getMesh(int i);
        ModelTexture* getModelTexture(int i);
        int getOrientationCount() { return m_effectParams->randomOrientations; }
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
            int randomOrientations,
            float variance,
            float rate,
            float angle,
            bool loop,
            float magnitude,
            Vector<glm::vec3>* accelerators,
            Vector<Mesh *>* meshes,
            Vector<ModelTexture *>* textures
            );

    private:
        Particle * makeParticle(int i);
        void initParticle(Particle *p, int i, int meshID, int modelTextureID);
        void initPosition(Particle *p);
        void initVelocity(Particle *p);
        void removeActiveParticle(int i);
        void addActiveParticle(int i);
        int meshSelect(ParticleEffect::Effect effect, int i);
        int modelTextureSelect(ParticleEffect::Effect effect, int i);
        Vector<ParticleEffect::Particle*> generateParticles();
        Vector<glm::vec3> * getActiveParticlePositions();
        Vector<int> getActiveMap();
        float getNextActivation();
        void updatePosition(Particle* p, float dt);
        void updateActiveParticles(float dt);
        void sphereMotion(Particle* p);
        void coneMotion(Particle* p);
        void diskMotion(Particle* p);
        

};

#endif 
