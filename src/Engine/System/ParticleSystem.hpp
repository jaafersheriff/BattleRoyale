#pragma once



#include "glm/glm.hpp"

#include "Util/Memory.hpp"



class ParticleComponent;
class Scene;
class GameObject;
class SpatialComponent;
class ParticleAssasinComponent;
class DiffuseShader;
class ShadowDepthShader;



class ParticleSystem {

    friend Scene;
    friend DiffuseShader;
    friend ShadowDepthShader;

    public:

        static void init();
        static void update(float dt);
    
    public:

        static const int k_maxNParticles;
        static const int k_maxVariations;
        static const float k_minScaleFactor;
        static const float k_maxScaleFactor;

        // Adds a particle effect anchored to the given spatial
        static ParticleComponent & addBodyExplosionPC(SpatialComponent & spatial);
        static ParticleComponent & addWaterFountainPC(SpatialComponent & spatial);
        static ParticleComponent & addSodaGrenadePC(SpatialComponent & spatial);

    private:

        static const Vector<ParticleComponent *> & s_particleComponents;
        static const Vector<ParticleAssasinComponent *> & s_particleAssasinComponents;
        static Vector<glm::mat4> m_variationMs;
        static Vector<glm::mat3> m_variationNs;

};