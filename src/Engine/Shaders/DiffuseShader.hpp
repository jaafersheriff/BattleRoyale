/* Basic diffuse shader */
#pragma once
#ifndef _DIFFUSE_SHADER_HPP_
#define _DIFFUSE_SHADER_HPP_

#include "Shader.hpp"

class CameraComonent;

class DiffuseShader : public Shader {
    public:
        DiffuseShader(const String & vertFile, const String & fragFile, const glm::vec3 & light);

        bool init();
        virtual void render(const CameraComponent * camera, const Vector<Component *> &) override;

        const glm::vec3 * lightDir;

        /* Wire frame */
        bool isWireFrame() const { return showWireFrame; }
        void toggleWireFrame() { showWireFrame = !showWireFrame; }

        /* Toon shading */
        bool isToon() const { return showToon; }
        void toggleToon() { showToon = !showToon; }
        float getSilAngle() const { return silAngle; }
        void setSilAngle(float in) { this->silAngle = in; }
        float getCells() const { return numCells; }
        void setCells(float in) { this->numCells = in; }

    private:
        bool showWireFrame = false;
        bool showToon = false;
        float silAngle = 0.f;
        float numCells = 1.f;
};

#endif 