#pragma once
#ifndef _DIFFUSE_RENDER_COMPONENT
#define _DIFFUSE_RENDER_COMPONENT

#include "Component/Component.hpp"

#include "Model/ModelTexture.hpp"
#include "Util/Geometry.hpp"



class RenderSystem;
class Mesh;
class SpatialComponent;
class DiffuseShader;



class DiffuseRenderComponent : public Component {

    friend Scene;
    friend DiffuseShader;

    private:

        static Sphere detEnclosingSphere(const Mesh & mesh);

    protected: // only scene or friends can create component

        DiffuseRenderComponent(GameObject & gameObject, const SpatialComponent & spatial, int pid, Mesh & mesh, const ModelTexture & texture, bool toon, glm::vec2 tiling);

    public:

        DiffuseRenderComponent(DiffuseRenderComponent && other) = default;

        virtual SystemID systemID() const override { return SystemID::render; };

        virtual void update(float dt) override;

        const Mesh & mesh() const { return m_mesh; }

        const ModelTexture & modelTexture() const { return m_modelTexture; }

        bool isToon() const  { return m_isToon; }

        const glm::vec2 & tiling const { return m_tiling; }

        const Sphere & enclosingSphere() const { return m_transSphere; }

    private:

        int m_pid;
        const Mesh & m_mesh;
        ModelTexture m_modelTexture;
        bool m_isToon;
        glm::vec2 m_tiling;
        const SpatialComponent * m_spatial;
        Sphere m_sphere;
        Sphere m_transSphere;
};

#endif
