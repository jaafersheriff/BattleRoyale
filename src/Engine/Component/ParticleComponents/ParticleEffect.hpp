#pragma once
#ifndef _PARTICLEEFFECT_HPP_
#define _PARTICLEEFFECT_HPP_

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/random.hpp"
#include "Model/Mesh.hpp"
#include "Model/ModelTexture.hpp"

class ParticleSystem;

class ParticleEffect {
    friend ParticleSystem;

    public:
        enum Type {
            SPHERE, 
            CONE,
            DISK
        };

        enum Effect {
            BLOOD_SPRAY,
            WATER_FOUNTAIN,
            SODA_GRENADE,
            BODY_EXPLOSION,
            SIRACHA_FLAMETHROWER
        };

        struct Particle{
            int i;
            bool active = false;
            float life = 0.0f;
            glm::vec3 position;
            glm::vec3 velocity;
            int orientationID;
            int meshID;
            int modelTextureID;
        };

        struct EffectParams {
            ParticleEffect::Type type;
            int n = 0; //Can be how many to spawn or, if rate is enable, the limit of particle count
            float effectDuration = 0.0f;
            float particleDuration = 0.0f;
            int orientations = 0; //How many different random orientations can the particle have, 0 means just base.
            bool randomDistribution = false; // Random vector vs even distribution except for cone.
            float variance = 0.0f; //random direction. keep low, like less than 0.5f
            float rate = 0.0f; // rate at which particles spawn (particles/sec), 0 if all spawn at once
            float angle = 2 * glm::pi<float>(); // default to 360 degrees
            bool loop = false;
            float magnitude = 1.0f;
            float attenuation = 1.0f; // Factor of how fast a particle decelerates. Keep high 0.8-1.0
            Vector<glm::vec3>* accelerators;
            Vector<Mesh *>* meshes;
            Vector<ModelTexture *>* textures;
        };

    public:
        ParticleEffect(EffectParams *ep, const glm::vec3 & anchor);
        ParticleEffect(EffectParams *ep, const glm::vec3 & anchor, const glm::vec3 & direction);
        ParticleEffect(EffectParams *ep, const glm::vec3 & anchor, const glm::vec3 & direction, const glm::vec3 & velocity);
   
    private:
        EffectParams * m_effectParams;
        glm::vec3 m_offset;
        glm::vec3 m_anchor;
        glm::vec3 m_direction;
        glm::vec3 m_velocity;
        Vector<Particle*> m_particles;
        float m_life;
        Vector<glm::vec3> * m_activeParticlePositions;
        Vector<int> * m_activeParticleOrientationIDs;
        Vector<int> m_activeMap;
        float m_nextActivation;
        float m_attenuationRate;
        float m_nextAttenuation;


    public:
        void update(float dt);
        void update(float dt, glm::vec3 anchor);
        void update(float dt, glm::vec3 anchor, glm::vec3 direction);
        Mesh* getMesh(int i);
        ModelTexture* getModelTexture(int i);
        int getOrientationCount() { return m_effectParams->orientations; }
        Vector<glm::vec3> * ActiveParticlePositions() {return m_activeParticlePositions; }
        Vector<int> * ActiveParticleOrientationIDs() { return m_activeParticleOrientationIDs; }
        glm::vec3   anchor()    { return m_anchor; }
        int         Count()     { return m_effectParams->n; }
        float       Life()      { return m_life; }
        float       Duration()  { return m_effectParams->effectDuration; }
        static EffectParams* createEffectParams(
            ParticleEffect::Type type,
            int n,
            float effectDuration,
            float particleDuration,
            int orientations,
            bool randomDistribution,
            float variance,
            float rate,
            float angle,
            bool loop,
            float magnitude,
            float attenuation,
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
        Vector<ParticleEffect::Particle*> generateParticles();
        Vector<glm::vec3> * getActiveParticlePositions();
        Vector<int> * getActiveParticleOrientationIDs();
        Vector<int> getActiveMap();
        int getAttenuations(float dt);
        float getNextActivation();
        void updatePosition(Particle* p, float dt, float attenuationFactor);
        void updateActiveParticles(float dt);
        void sphereMotion(Particle* p);
        void coneMotion(Particle* p);
        void diskMotion(Particle* p);
        glm::vec3 getU(glm::vec3 direction);
        float getRandom(float low, float high);
        

};

#endif 
