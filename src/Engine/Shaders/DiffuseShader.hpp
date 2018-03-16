/* Basic diffuse shader */
#pragma once
#ifndef _DIFFUSE_SHADER_HPP_
#define _DIFFUSE_SHADER_HPP_

#include "Shader.hpp"

class DiffuseShader : public Shader {
    public:
        DiffuseShader(const String & vertFile, const String & fragFile);

        bool init();
        virtual void render(const CameraComponent * camera) override;

        /* Wire frame */
        bool isWireFrame() const { return showWireFrame; }
        void toggleWireFrame() { showWireFrame = !showWireFrame; }

        /* Toon shading */
        bool isToon() const { return showToon; }
        void toggleToon() { showToon = !showToon; }
        float getSilAngle() const { return silAngle; }
        void setSilAngle(float in) { this->silAngle = in; }
        unsigned int getCells() const { return numCells; }
        void setCells(unsigned int in);
        float getCellIntensity(unsigned int i) { return cellIntensities[i]; }
        void setCellIntensity(unsigned int i, float f);
        float getCellDiffuseScale(unsigned int i) { return cellDiffuseScales[i]; }
        void setCellDiffuseScale(unsigned int i, float f);
        float getCellSpecularScale(unsigned int i) { return cellSpecularScales[i]; }
        void setCellSpecularScale(unsigned int i, float f);

    private:
        /* Wire frame */
        bool showWireFrame = false;

        /* Toon shading */
        bool showToon = false;
        float silAngle = 0.f;
        unsigned int numCells = 1;
        Vector<float> cellIntensities;
        Vector<float> cellDiffuseScales;
        Vector<float> cellSpecularScales;
        unsigned int cellIntensitiesTexture;
        unsigned int cellDiffuseScalesTexture;
        unsigned int cellSpecularScalesTexture;

        /* Shadows */
        glm::mat4 * L;

        // Particles
        unsigned int particlesVAO;
        unsigned int particlesVBO;
};

#endif 