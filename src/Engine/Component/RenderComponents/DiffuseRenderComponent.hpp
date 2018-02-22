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

        DiffuseRenderComponent(int pid, Mesh & mesh, ModelTexture texture, bool t) :
            pid(pid),
            mesh(&mesh),
            modelTexture(texture),
            isToon(t)
        {}

    public:

    virtual SystemID systemID() const override { return SystemID::render; };

    public:

        int pid = -1;
        Mesh * mesh;
        ModelTexture modelTexture;
        bool isToon = false;
};

#endif