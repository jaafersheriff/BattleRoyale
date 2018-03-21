#pragma once
#ifndef _HEALTH_SHADER_HPP_
#define _HEALTH_SHADER_HPP_

#include "Shader.hpp"

class Mesh;

class HealthShader : public Shader {
    public:
        HealthShader(const String & vertName, const String & fragName);

        bool init();

        virtual void render(const CameraComponent * camera) override;

        glm::vec2 m_size;
    private:
        bool initQuad();
        unsigned int m_quadVAO, m_quadVBO;
};

#endif