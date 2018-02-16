/* Basic free-floating camera class */
#pragma once
#ifndef _CAMERA_COMPONENT_HPP_
#define _CAMERA_COMPONENT_HPP_

#include "glm/glm.hpp"

#include "Util/Geometry.hpp"
#include "Component/Component.hpp"

class GameLogicSystem;
class SpatialComponent;

class CameraComponent : public Component {

    friend Scene;

    protected: // only scene can create component

        CameraComponent(SpatialComponent & spatial, float fov);

    public:
        
        /* Derived functions */

        virtual SystemID systemID() const override { return SystemID::gameLogic; };

        void init();

        void update(float dt);

        // sets the camera to look at p
        void lookAt(const glm::vec3 & p);

        // sets the camera to look in dir
        void lookInDir(const glm::vec3 & dir);

        // sets the camera angle
        // +yaw is radians right, -yaw is radians left
        // +pitch is radians up, -pitch is radians down
        // if relative is true, these angles are applied as deltas to the
        // current orientation. if relative is false, these angles are absolute
        // from the +x axis as forward
        void angle(float yaw, float pitch, bool relative);

        void setFOV(float fov);

        void setNearFar(float near, float far);

        const bool sphereInFrustum(const Sphere & sphere) const;
        float theta() const { return m_theta; }
        float phi() const { return m_phi; }
        float fov() const { return m_fov; }

        glm::vec3 getLookDir() const;
        
        const glm::mat4 & getView() const;
        const glm::mat4 & getProj() const;

    private:

        void setUVW();

        void detAngles();

        void detView() const;
        void detProj() const;
        void detFrustum() const;

    private:

        SpatialComponent & m_spatial;
        /* Describes the rotation of the camera */
        float m_theta, m_phi;
        /* field of view, near plane, and far plane */
        float m_fov;

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