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

protected: // only scene or friends can create component

    ParticleRenderComponent(GameObject & gameObject, int pid, Mesh & mesh, const ModelTexture & texture, bool toon,
        ParticleEffect::Effect effect) :
        Component(gameObject),
        pid(pid),
        mesh(&mesh),
        modelTexture(texture),
        isToon(toon),
        effect(effect)
    {}

public:

    ParticleRenderComponent(ParticleRenderComponent && other) = default;

    virtual SystemID systemID() const override { return SystemID::render; };

public:

    int pid;
    Mesh * mesh;
    ModelTexture modelTexture;
    bool isToon;
    ParticleEffect::Effect effect;

};

#endif