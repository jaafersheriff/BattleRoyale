#pragma once
#include "Engine/Scene/Scene.hpp"
#include "Component/ParticleComponents/ParticleEffect.hpp"
#include "Component/ParticleComponents/ParticleComponent.hpp"
class ParticleSystem {

    friend Scene;

    public:

    static void init();
    static void update(float dt);

    private:

    static Vector<ParticleComponent *> & components;

};