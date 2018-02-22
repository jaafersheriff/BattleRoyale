#pragma once
#ifndef _DIFFUSE_RENDER_COMPONENT
#define _DIFFUSE_RENDER_COMPONENT

#include "Component/Component.hpp"

#include "Model/Mesh.hpp"
#include "Model/ModelTexture.hpp"

class RenderSystem;

class DiffuseRenderComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

        DiffuseRenderComponent(int pid, Mesh & mesh, ModelTexture texture, bool toon) :
            pid(pid),
            mesh(&mesh),
            modelTexture(texture),
            isToon(toon)
        {}

    public:

    virtual SystemID systemID() const override { return SystemID::render; };

    public:

        int pid;
        Mesh * mesh;
        ModelTexture modelTexture;
        bool isToon;
};

#endif
