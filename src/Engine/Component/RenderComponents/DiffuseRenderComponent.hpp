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
class ShadowDepthShader;



class DiffuseRenderComponent : public Component {

    friend Scene;
    friend DiffuseShader;
    friend ShadowDepthShader;

    private:

        static Sphere detEnclosingSphere(const Mesh & mesh);

    protected: // only scene or friends can create component

        DiffuseRenderComponent(
            GameObject & gameObject, const SpatialComponent & spatial,
            const Mesh & mesh, const ModelTexture & texture,
            bool toon, const glm::vec2 & tiling, bool isNeon
        );

    public:

        DiffuseRenderComponent(DiffuseRenderComponent && other) = default;

        virtual void update(float dt) override;

        const Mesh & mesh() const { return m_mesh; }

        const ModelTexture & modelTexture() const { return m_modelTexture; }

        bool isToon() const  { return m_isToon; }

        bool isNeon() const { return m_isNeon; }

        const glm::vec2 & tiling() const { return m_tiling; }

        const Sphere & enclosingSphere() const { return m_transSphere; }

    private:

        const Mesh & m_mesh;
        ModelTexture m_modelTexture;
        bool m_isToon;
        glm::vec2 m_tiling;
        bool m_isNeon;
        const SpatialComponent * m_spatial;
        Sphere m_sphere;
        Sphere m_transSphere;
};

#endif
