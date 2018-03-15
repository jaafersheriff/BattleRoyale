#pragma once



#ifndef _CAMERA_COMPONENT_HPP_
#define _CAMERA_COMPONENT_HPP_



#include "glm/glm.hpp"

#include "Util/Geometry.hpp"
#include "Component/Component.hpp"
#include "Component/SpatialComponents/Orientable.hpp"



class GameSystem;
class SpatialComponent;



// Camera is relative to a base, usually the player.
// Both the camera and the base have an orientation. The camera's angles are
// relative to the base. The camera's uvw vectors are absolute.
class CameraComponent : public Component, public Orientable {

    friend Scene;

    protected: // only scene or friends can create component

        CameraComponent(GameObject & gameObject, float fov, float near, float far);

    public:

        CameraComponent(CameraComponent && other) = default;

    protected:

        virtual void init() override;

    public:
        
        /* Derived functions */

        virtual SystemID systemID() const override { return SystemID::game; };

        void update(float dt);

        // sets the camera to look at p
        void lookAt(const glm::vec3 & p);

        // sets the camera to look in dir
        void lookInDir(const glm::vec3 & dir);

        // sets the camera angle
        // +theta is radians left, -theta is radians left
        // +phi is radians down, -pitch is radians up
        // if relative is true, these angles are applied as deltas to the
        // current orientation. if relative is false, these angles are absolute
        // from the orientation of the base
        void angle(float theta, float phi, bool relative, bool silently = true);

        void setFOV(float fov);

        void setNearFar(float near, float far);

        const bool sphereInFrustum(const Sphere & sphere) const;
        float theta() const { return m_theta; }
        float phi() const { return m_phi; }
        float fov() const { return m_fov; }
        float near() const { return m_near; }
        float far() const { return m_far; }

        glm::vec3 getLookDir() const;
        
        const glm::mat4 & getView() const;
        const glm::mat4 & getProj() const;

    private:

        void detUVW();

        void detAngles();

        void detView() const;
        void detProj() const;
        void detFrustum() const;

    private:

        SpatialComponent * m_spatial;
        float m_theta, m_phi; // rotation of camera relative to base
        float m_fov; // field of view
        float m_near, m_far; // distance to near and far planes

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