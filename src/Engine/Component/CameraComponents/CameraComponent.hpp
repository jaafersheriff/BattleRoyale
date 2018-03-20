#pragma once



#ifndef _CAMERA_COMPONENT_HPP_
#define _CAMERA_COMPONENT_HPP_



#include "glm/glm.hpp"

#include "Util/Geometry.hpp"
#include "Component/Component.hpp"
#include "Component/SpatialComponents/Orientable.hpp"



class GameSystem;
class SpatialComponent;



// Camera simply provides additional functionality wrapping around a spatial
// component. All location and orientation information is in the spatial
class CameraComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

        CameraComponent(GameObject & gameObject, float fov, float near, float far, SpatialComponent * spatial = nullptr);
        CameraComponent(GameObject & gameObject, glm::vec2 horiz, glm::vec2 vert, float near, float far, SpatialComponent * spatial = nullptr);

    public:

        CameraComponent(CameraComponent && other) = default;

    protected:

        virtual void init() override;

    public:

        void setFOV(float fov);
        void setNearFar(float near, float far);
        void setOrthoBounds(glm::vec2, glm::vec2);

        const bool sphereInFrustum(const Sphere & sphere) const;
        float fov() const { return m_fov; }
        float near() const { return m_near; }
        float far() const { return m_far; }
        glm::vec2 hBounds() const { return m_hBounds; }
        glm::vec2 vBounds() const { return m_vBounds; }

        glm::vec3 getLookDir() const;
        
        const glm::mat4 & getView() const;
        const glm::mat4 & getProj() const;

        const SpatialComponent & spatial() const { return *m_spatial; }

    private:

        void detView() const;
        void detProj() const;
        void detFrustum() const;

    private:

        SpatialComponent * m_spatial;
        float m_fov;                // field of view
        float m_near, m_far;        // distance to near and far planes
        bool m_isOrtho;
        glm::vec2 m_hBounds, m_vBounds;

        // DONT USE THESE DIRECTLY, CALL GETVIEW OR GETPROJ
        mutable glm::mat4 m_viewMat;
        mutable glm::mat4 m_projMat;
        mutable bool m_viewMatValid;
        mutable bool m_projMatValid;
        
        mutable glm::vec4 m_frustumLeft;
        mutable glm::vec4 m_frustumRight;
        mutable glm::vec4 m_frustumBottom;
        mutable glm::vec4 m_frustumTop;
        mutable glm::vec4 m_frustumNear;
        mutable glm::vec4 m_frustumFar;
        mutable bool m_frustumValid;
};

#endif