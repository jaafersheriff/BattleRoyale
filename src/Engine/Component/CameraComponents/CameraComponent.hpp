/* Basic free-floating camera class */
#pragma once
#ifndef _CAMERA_COMPONENT_HPP_
#define _CAMERA_COMPONENT_HPP_

#include "Component/Component.hpp"

#include "glm/glm.hpp"

class GameLogicSystem;

class CameraComponent : public Component {

    public:

    using SystemClass = GameLogicSystem;

    public:
        /* Constructor */
        CameraComponent(float fov, float aspect, float near, float far) :
            fov(fov),
            aspect(aspect),
            near(near),
            far(far) {
        }
        
        /* Derived functions */
        void init();
        void update(float dt);

        /* Member functions */
        const glm::mat4 & getView() const { return view; }
        const glm::mat4 & getProj() const { return projection; }

        /* Member vars */
        /* w = forwards-backwards of camera */
        /* v = up-down of camera */
        /* u = left-right of camera */
        glm::vec3 u, v, w;
        /* Where the camera is looking at in world space */
        glm::vec3 lookAt;
        /* Describes the rotation of the camera */
        double phi, theta;

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
        /* Projection */
        float fov, aspect, near, far;
        glm::mat4 projection;

        /* View */
        glm::mat4 view;
};

#endif