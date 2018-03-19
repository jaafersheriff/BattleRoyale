#pragma once
#ifndef _SQUARE_SHADER_HPP_
#define _SQUARE_SHADER_HPP_

#include "Shader.hpp"
#include "Loader/Loader.hpp"

class DiffuseRenderComponent;
class CameraComponent;

class PostProcessShader : public Shader {

    public:

    PostProcessShader(const String & vertName, const String & fragName);

    virtual bool init() override;

    virtual void render(const CameraComponent * camera) override;

    private:

    unsigned int s_vaoHandle;
    unsigned int s_vboHandle;
    unsigned int s_iboHandle;

    // UI textures
    Texture *tex_pizza;
};

#endif
