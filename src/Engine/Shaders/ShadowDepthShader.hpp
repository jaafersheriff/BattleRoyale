/* Renders scene's depth to an FBO from the POV of the light */
#pragma once
#ifndef _SHADOW_DEPTH_SHADER_HPP_
#define _SHADOW_DEPTH_SHADER_HPP_

#include "Shader.hpp"

class ShadowDepthShader : public Shader {
    public:
        ShadowDepthShader(const String & vertName, const String & fragName, int width, int height, glm::vec3 & lightdir);

        bool init();
        virtual void render(const CameraComponent * camera, const Vector<Component *> &) override;
        
    private:
        void initFBO();

        glm::vec3 & lightDir;
        
        glm::mat4 L;
        GLuint fboHandle;
        GLuint fboTexture;
        int mapWidth, mapHeight;
};

#endif