/* Basic free-floating camera class */
#pragma once
#ifndef _CAMERA_COMPONENT_HPP_
#define _CAMERA_COMPONENT_HPP_

#include "glm/glm.hpp"

#include "Component/Component.hpp"
#include "Util/Geometry.hpp"

class GameLogicSystem;

class CameraComponent : public Component {

    friend Scene;

    protected: // only scene can create component

        CameraComponent(float fov, float near, float far);

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

        const bool sphereInFrustum(const Sphere & sphere) const;

        const glm::vec3 & u() const { return m_u; }
        const glm::vec3 & v() const { return m_v; }
        const glm::vec3 & w() const { return m_w; }
        float theta() const { return m_theta; }
        float phi() const { return m_phi; }
        float fov() const { return m_fov; }
        float near() const { return m_near; }
        float far() const { return m_far; }
        
        const glm::mat4 & getView() const;
        const glm::mat4 & getProj() const;

    private:

        void detUVW();
        void detView() const;
        void detProj() const;

    public:

        /* Data about the view frustum */
        float
            farPlaneWidth, farPlaneHeight,
            nearPlaneWidth, nearPlaneHeight;
        /* Planes can be described by a point in space and a normal */
        glm::vec3
            farPlanePoint, farPlaneNormal,
            nearPlanePoint, nearPlaneNormal,
            topPlanePoint, topPlaneNormal,
            bottomPlanePoint, bottomPlaneNormal,
            leftPlanePoint, leftPlaneNormal,
            rightPlanePoint, rightPlaneNormal;

    private:

        /* u, v, and w define a set of orthonormal basis unit vectors */
        /* u points right (+x) */
        /* v points up (+y) */
        /* w points back (+z) */
        glm::vec3 m_u, m_v, m_w;
        /* Describes the rotation of the camera */
        float m_theta, m_phi;
        /* field of view, near plane, and far plane */
        float m_fov, m_near, m_far;

        // DONT USE THESE DIRECTLY, CALL GETVIEW OR GETPROJ
        mutable glm::mat4 m_viewMat;
        mutable glm::mat4 m_projMat;
        mutable bool m_viewMatValid;
        mutable bool m_projMatValid;
};

#endif