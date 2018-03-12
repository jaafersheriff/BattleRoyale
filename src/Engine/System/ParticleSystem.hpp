#pragma once
#include "Engine/Scene/Scene.hpp"
#include "Component/ParticleComponents/ParticleComponent.hpp"

class ParticleShader;

class ParticleSystem {

    friend Scene;
    friend ParticleShader;

    public:

        static void init();
        static void update(float dt);

    private:
        static const Vector<ParticleComponent *> & s_particleComponents;

};