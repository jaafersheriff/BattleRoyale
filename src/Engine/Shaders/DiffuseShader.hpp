/* Basic diffuse shader */
#pragma once
#ifndef _DIFFUSE_SHADER_HPP_
#define _DIFFUSE_SHADER_HPP_

#include "Shader.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"

class DiffuseShader : public Shader {
    public:
        DiffuseShader(std::string vert, std::string frag, CameraComponent *cam, glm::vec3 *light) :
            Shader(vert, frag),
            camera(cam),
            lightPos(light)
        {}

        bool init();
        void render(std::string, std::vector<Component *> *);

        CameraComponent *camera;
        glm::vec3 *lightPos;
};

#endif 