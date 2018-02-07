#pragma once
#ifndef _DIFFUSE_RENDERABLE_COMPONENT
#define _DIFFUSE_RENDERABLE_COMPONENT

#include "Component/Component.hpp"

#include "Model/Mesh.hpp"
#include "Model/ModelTexture.hpp"

class DiffuseRenderableComponent : public Component {
    public:
        DiffuseRenderableComponent(int pid, Mesh *mesh, ModelTexture texture) :
            pid(pid),
            mesh(mesh),
            modelTexture(texture)
        {}

        int pid = -1;;
        Mesh *mesh = nullptr;
        ModelTexture modelTexture;
};

#endif