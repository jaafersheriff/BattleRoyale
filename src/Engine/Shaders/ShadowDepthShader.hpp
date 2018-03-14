/* Renders scene's depth to an FBO from the POV of the light */
#pragma once
#ifndef _SHADOW_DEPTH_SHADER_HPP_
#define _SHADOW_DEPTH_SHADER_HPP_

#include "Shader.hpp"

class ShadowDepthShader : public Shader {
    public:
        ShadowDepthShader(const String & vertName, const String & fragName);

        bool init();

        virtual void render(const CameraComponent * camera, const Vector<DiffuseRenderComponent *> &) override;
        
        GLuint getShadowMapTexture() { return fboTexture; }
        const glm::mat4 & getL() { return L; }
    private:
        void initFBO();
        
        glm::mat4 L;
        GLuint fboHandle;
        GLuint fboTexture;
        int mapWidth, mapHeight;
};

#endif