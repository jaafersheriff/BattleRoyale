/* Basic diffuse shader */
#pragma once
#ifndef _DIFFUSE_SHADER_HPP_
#define _DIFFUSE_SHADER_HPP_

#include "Shader.hpp"

class DiffuseShader : public Shader {
    public:
        DiffuseShader(std::string vert, std::string frag, glm::vec3 *, glm::vec3 *);

        void render(std::string, std::vector<Component *> *);

        glm::vec3 *cameraPos;
        glm::vec3 *lightPos;
};

#endif 