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
        glm::vec3 u, v, w;
        glm::vec3 lookAt;
        double phi, theta;
    private:
        /* Projection */
        float fov, aspect, near, far;
        glm::mat4 projection;

        /* View */
        glm::mat4 view;
};

#endif