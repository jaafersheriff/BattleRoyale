/* Basic free-floating camera class */
#pragma once
#ifndef _CAMERA_COMPONENT_HPP_
#define _CAMERA_COMPONENT_HPP_

#include "Component/Component.hpp"

#include "glm/glm.hpp"

class CameraComponent : Component {
    public:
        /* Constructor */
        CameraComponent(float fov, float aspect, float near, float far, glm::vec3 pos) :
            fov(fov),
            aspect(aspect),
            near(near),
            far(far),
            position(pos) 
        {}
        
        /* Derived functions */
        void init();
        void update(float dt);

        /* Member functions */
        const glm::mat4 &getView() { return view; }
        const glm::mat4 &getProj() { return projection; }
    
        /* Member vars */
        double phi, theta;
        glm::vec3 u, v, w;
        glm::vec3 position;
        glm::vec3 lookAt;
    private:
        /* Projection */
        float fov, aspect, near, far;
        glm::mat4 projection;

        /* View */
        glm::mat4 view;
};

#endif