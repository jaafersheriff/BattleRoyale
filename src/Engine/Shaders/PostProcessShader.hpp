#pragma once
#ifndef _SQUARE_SHADER_HPP_
#define _SQUARE_SHADER_HPP_

#include "Shader.hpp"

class DiffuseRenderComponent;
class CameraComponent;

class PostProcessShader : public Shader {

  public:

    PostProcessShader(const String & vertName, const String & fragName);

    virtual bool init() override;

    virtual void render(const CameraComponent * camera) override;

  private:

    GLuint s_vaoHandle;
    GLuint s_vboHandle;
    GLuint s_iboHandle;
};

#endif
