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
        GLuint cellIntensitiesTexture;
        GLuint cellDiffuseScalesTexture;
        GLuint cellSpecularScalesTexture;
};

#endif 