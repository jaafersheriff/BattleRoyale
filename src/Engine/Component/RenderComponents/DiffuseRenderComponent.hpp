#pragma once
#ifndef _DIFFUSE_RENDER_COMPONENT
#define _DIFFUSE_RENDER_COMPONENT

#include "Component/Component.hpp"

#include "Model/Mesh.hpp"
#include "Model/ModelTexture.hpp"

class RenderSystem;

class DiffuseRenderComponent : public Component {

    friend Scene;

    public:

    using SystemClass = RenderSystem;

    protected: // only scene can create component

        DiffuseRenderComponent(int pid, Mesh & mesh, ModelTexture texture) :
            pid(pid),
            mesh(&mesh),
            modelTexture(texture)
        {}

    public:

        int pid = -1;
        Mesh * mesh;
        ModelTexture modelTexture;
};

#endif