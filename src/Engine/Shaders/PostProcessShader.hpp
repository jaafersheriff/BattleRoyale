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

    void setScreenTone(const glm::vec3 & tone);

  private:

    unsigned int m_vaoHandle;
    unsigned int m_vboHandle;
    unsigned int m_iboHandle;

    glm::vec3 m_screenTone;

    // UI textures
    Texture *tex_pizza;
};

#endif
