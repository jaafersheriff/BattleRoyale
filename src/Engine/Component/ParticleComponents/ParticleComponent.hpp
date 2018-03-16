#pragma once



#include "Component/Component.hpp"
#include "Model/ModelTexture.hpp"



class Mesh;
class ParticleShader;



struct Particle {

    glm::vec3 position;
    unsigned int matrixI;
    glm::vec3 velocity;
    float life;

    Particle(
        const glm::vec3 & position,
        const glm::vec3 & velocity,
        unsigned int matrixI
    );

};



// Defines behavior for giving particles initial velocities
class ParticleInitializer {

    public:

        ParticleInitializer(float minSpeed, float maxSpeed) :
            m_minSpeed(minSpeed),
            m_maxSpeed(maxSpeed)
        {}

        // The result should be in respect to standard uvw frame where -w is "forward"
        virtual void initialize(Particle & p, unsigned int i, unsigned int n) = 0;

    protected:

        float m_minSpeed;
        float m_maxSpeed;

};

class SphereParticleInitializer : public ParticleInitializer {

    public:

        SphereParticleInitializer(float minSpeed, float maxSpeed, float randomDistribution) :
            ParticleInitializer(minSpeed, maxSpeed),
            m_randomDistribution(randomDistribution)
        {}

        virtual void initialize(Particle & p, unsigned int i, unsigned int n) override;

    protected:

        bool m_randomDistribution;

};

class ConeParticleInitializer : public ParticleInitializer {

    public:

        ConeParticleInitializer(float minSpeed, float maxSpeed, float angle, bool randomDistribution) :
            ParticleInitializer(minSpeed, maxSpeed),
            m_angle(angle),
            m_randomDistribution(randomDistribution)
        {}
    
        virtual void initialize(Particle & p, unsigned int i, unsigned int n) override;

    protected:
    
        float m_angle;
        bool m_randomDistribution;

};

class DiskParticleInitializer : public ParticleInitializer {

    public:

        DiskParticleInitializer(float minSpeed, float maxSpeed, float angle, bool randomDistribution) :
            ParticleInitializer(minSpeed, maxSpeed),
            m_angle(angle),
            m_randomDistribution(randomDistribution)
        {}
    
        virtual void initialize(Particle & p, unsigned int i, unsigned int n) override;

    protected:
    
        float m_angle;
        bool m_randomDistribution;

};



class ParticleUpdater {

    public:

        virtual void update(Vector<Particle> & particles, float dt) = 0;

};

class LinearParticleUpdater : public ParticleUpdater {

    public:

        virtual void update(Vector<Particle> & particles, float dt) override;

};

class GravityParticleUpdater : public ParticleUpdater {

    public:

        virtual void update(Vector<Particle> & particles, float dt) override;

};

class AttenuationParticleUpdater : public ParticleUpdater {

    public:

        AttenuationParticleUpdater(float attenutation) :
            m_attenuation(attenutation)
        {}

        virtual void update(Vector<Particle> & particles, float dt) override;

    protected:

        float m_attenuation;

};



class ParticleComponent : public Component {

        friend Scene;
        friend ParticleShader;

    public:

        ParticleComponent(GameObject & gameObject,
            UniquePtr<ParticleInitializer> && initializer,
            UniquePtr<ParticleUpdater> && updater,
            int maxN,
            float rate,
            float duration,
            bool loop,
            const Mesh & mesh,
            float scale,
            const ModelTexture & modelTexture,
            const SpatialComponent & anchor,
            bool variation,
            bool fade
        );
        ParticleComponent(ParticleComponent && other) = default;

        virtual ~ParticleComponent() = default;
    
    protected:

        virtual void init() override;
    
    public:

        virtual SystemID systemID() const override { return SystemID::particle; }

        virtual void update(float dt) override;

        int count() { return int(m_particles.size()); }
        bool finished() { return !m_loop && m_particles.empty(); }

    protected:

        UniquePtr<ParticleInitializer> m_initializer;
        UniquePtr<ParticleUpdater> m_updater;
        int m_maxN; // maximum number of particles active at any time
        float m_rate; // rate at which particles spawn (particles/sec), 0 if all spawn at once
        float m_duration; // length of time particle last
        bool m_loop; // whether the effect should loop
        const Mesh & m_mesh;
        float m_scale; // scale factor for mesh
        ModelTexture m_modelTexture;
        const SpatialComponent & m_anchor; // particles will spawn following this
        bool m_variation; // particles have varied scale and orientation
        bool m_fade; // particles will get smaller over the span of their life

        Vector<Particle> m_particles;
        float m_timeAccum;
        unsigned int m_runningParticleID;
        bool m_firstSpawn;
        
};
