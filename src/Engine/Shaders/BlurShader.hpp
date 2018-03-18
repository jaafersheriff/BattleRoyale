#pragma once
#ifndef _BLUR_SHADER_HPP_
#define _BLUR_SHADER_HPP_

#include "Shader.hpp"

class BlurShader : public Shader {

    public:

        BlurShader(const String & vertName, const String & fragName);

        virtual bool init() override;

        virtual void render(const CameraComponent * camera) override;

        bool s_horizontal;

    private:

        unsigned int s_vaoHandle;
        unsigned int s_vboHandle;
        unsigned int s_iboHandle;
};

#endif