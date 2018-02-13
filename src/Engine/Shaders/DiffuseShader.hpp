/* Basic diffuse shader */
#pragma once
#ifndef _DIFFUSE_SHADER_HPP_
#define _DIFFUSE_SHADER_HPP_

#include "Shader.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"

class DiffuseShader : public Shader {
    public:
        DiffuseShader(const std::string & vert, const std::string & frag, const CameraComponent & cam, const glm::vec3 & light) :
            Shader(vert, frag),
            camera(&cam),
            lightPos(&light)
        {}

        bool init();
        void render(const std::string &, const std::vector<Component *> &);
        bool sphereInFrustum(glm::vec3 center, float radius, CameraComponent *);

        const CameraComponent * camera;
        const glm::vec3 * lightPos;
};

#endif 