/* Basic diffuse shader */
#pragma once
#ifndef _DIFFUSE_SHADER_HPP_
#define _DIFFUSE_SHADER_HPP_

#include "Shader.hpp"

class CameraComonent;

class DiffuseShader : public Shader {
    public:
        DiffuseShader(const std::string & vertFile, const std::string & fragFile, const glm::vec3 & light);

        bool init();
        virtual void render(const CameraComponent * camera, const Vector<Component *> &) override;

        const glm::vec3 * lightPos;

        bool isWireFrame() const { return showWireFrame; }
        void toggleWireFrame() { showWireFrame = !showWireFrame; }
    private:
        bool showWireFrame = false;
};

#endif 