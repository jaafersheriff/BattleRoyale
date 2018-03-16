#pragma once



#include "glm/glm.hpp"

#include "Util/Memory.hpp"



class ParticleShader;
class ParticleComponent;
class Scene;
class GameObject;
class SpatialComponent;



class ParticleSystem {

    friend Scene;
    friend ParticleShader;

    public:

        static void init();
        static void update(float dt);
    
    public:

        static const int k_maxNParticles;
        static const int k_maxVariations;
        static const float k_minScaleFactor;
        static const float k_maxScaleFactor;

        static ParticleComponent & addBodyExplosionPC(SpatialComponent & spatial);
        static ParticleComponent & addWaterFountainPC(SpatialComponent & spatial);
        static ParticleComponent & addSodaGrenadePC(SpatialComponent & spatial);

    private:

        static const Vector<ParticleComponent *> & s_particleComponents;
        static Vector<glm::mat4> m_variationMs;
        static Vector<glm::mat3> m_variationNs;

};