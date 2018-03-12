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

        DiffuseRenderComponent(GameObject & gameObject, Mesh & mesh, const ModelTexture & texture, bool toon) :
            Component(gameObject),
            mesh(&mesh),
            modelTexture(texture),
            isToon(toon),
            tiling(tiling)
        {}

    public:

        DiffuseRenderComponent(DiffuseRenderComponent && other) = default;

        virtual SystemID systemID() const override { return SystemID::render; };

    public:

        Mesh * mesh;
        ModelTexture modelTexture;
        bool isToon;
        glm::vec2 tiling;
};

#endif
