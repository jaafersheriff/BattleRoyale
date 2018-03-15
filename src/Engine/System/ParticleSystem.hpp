#pragma once

#include "Util/Memory.hpp"

class ParticleShader;
class ParticleComponent;
class Scene;

class ParticleSystem {

    friend Scene;
    friend ParticleShader;

    public:

        static void init();
        static void update(float dt);

    private:

        static const Vector<ParticleComponent *> & s_particleComponents;

};