/* Basic diffuse shader */
#pragma once
#ifndef _PARTICLE_SHADER_HPP_
#define _PARTICLE_SHADER_HPP_

#include "Shader.hpp"
#include "System/ParticleSystem.hpp"

class ParticleShader : public Shader {
    public:
        ParticleShader(const String & vertFile, const String & fragFile, const glm::vec3 & light);

        bool init();
        virtual void render(const CameraComponent * camera, const Vector<Component *> &) override;

        const glm::vec3 * lightDir;

        /* Wire frame */
        bool isWireFrame() const { return showWireFrame; }
        void toggleWireFrame() { showWireFrame = !showWireFrame; }

    private:
        /* Wire frame */
        bool showWireFrame = false;

};

#endif 