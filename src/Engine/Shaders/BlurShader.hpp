#pragma once
#ifndef _BLUR_SHADER_HPP_
#define _BLUR_SHADER_HPP_

#include "Shader.hpp"

class BlurShader : public Shader {

    public:

        BlurShader(const String & vertName, const String & fragName);

        virtual bool init() override;

        virtual void render(const CameraComponent * camera, const Vector<Component *> & components) override;

        bool s_Horizontal;

    private:

        GLuint s_vaoHandle;
        GLuint s_vboHandle;
        GLuint s_iboHandle;
};

#endif