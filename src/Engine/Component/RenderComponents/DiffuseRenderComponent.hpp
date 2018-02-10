#pragma once
#ifndef _DIFFUSE_RENDER_COMPONENT
#define _DIFFUSE_RENDER_COMPONENT

#include "Component/Component.hpp"

#include "Model/Mesh.hpp"
#include "Model/ModelTexture.hpp"

class RenderSystem;

class DiffuseRenderComponent : public Component {

    public:

    using SystemClass = RenderSystem;

    public:
        DiffuseRenderComponent(int pid, Mesh & mesh, ModelTexture texture) :
            pid(pid),
            mesh(&mesh),
            modelTexture(texture)
        {}

        int pid = -1;
        Mesh * mesh;
        ModelTexture modelTexture;
};

#endif