/* Basic diffuse shader */
#pragma once
#ifndef _DIFFUSE_SHADER_HPP_
#define _DIFFUSE_SHADER_HPP_

#include "Shader.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"

class DiffuseShader : public Shader {
    public:
        DiffuseShader(const std::string & vertFile, const std::string & fragFile, const CameraComponent & cam, const glm::vec3 & light);

        bool init();
        void render(const std::string &, const std::vector<Component *> &);

        const CameraComponent * camera;
        const glm::vec3 * lightPos;
};

#endif 