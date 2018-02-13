/* Basic free-floating camera class */
#pragma once
#ifndef _CAMERA_COMPONENT_HPP_
#define _CAMERA_COMPONENT_HPP_

#include "Component/Component.hpp"

#include "glm/glm.hpp"

class GameLogicSystem;

class CameraComponent : public Component {

    friend Scene;

    public:

    using SystemClass = GameLogicSystem;

    protected: // only scene can create component

        /* Constructor */
        CameraComponent(float fov, float near, float far) :
            fov(fov),
            near(near),
            far(far) {
        }

    public:
        
        /* Derived functions */
        void init();
        void update(float dt);

        /* Member functions */
        void setDirty() { isDirty = true; }
        const glm::mat4 & getView() const { return view; }
        const glm::mat4 & getProj() const { return projection; }

        /* Member vars */
        glm::vec3 u, v, w;
        glm::vec3 lookAt;
        double phi, theta;
    private:
        /* Denotes if camera has been moved */
        bool isDirty = false;

        /* Projection */
        float fov, near, far;
        glm::mat4 projection;

        /* View */
        glm::mat4 view;
};

#endif