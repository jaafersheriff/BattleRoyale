#pragma once
#pragma once
#ifndef _PARTICLE_RENDER_COMPONENT
#define _PARTICLE_RENDER_COMPONENT

#include "Component/Component.hpp"

#include "Model/Mesh.hpp"
#include "Model/ModelTexture.hpp"
#include "Component/ParticleComponents/ParticleEffect.hpp"

class RenderSystem;

class ParticleRenderComponent : public Component {

    friend Scene;
    //friend ParticleComponent;

protected: // only scene or friends can create component

    ParticleRenderComponent(GameObject & gameObject, int pid, bool toon,
        ParticleEffect::Effect effect) :
        Component(gameObject),
        pid(pid),
        isToon(toon),
        effect(effect)
    {}

public:

    ParticleRenderComponent(ParticleRenderComponent && other) = default;

    virtual SystemID systemID() const override { return SystemID::render; };

public:

    int pid;
    bool isToon;
    ParticleEffect::Effect effect;

};

#endif