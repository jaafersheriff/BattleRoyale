#pragma once
#ifndef _SQUARE_SHADER_HPP_
#define _SQUARE_SHADER_HPP_

#include "Shader.hpp"

class SquareShader : public Shader {
   public:
      SquareShader(const String & vertName, const String & fragName);

      bool init();
      virtual void render(const CameraComponent * camera, const Vector<Component *> & components) override;

      GLuint fboHandle;
      GLuint colorTexture;
      GLuint depthTexture;

      Vector<float> frameSquarePos;
      Vector<unsigned> frameSquareElem;
      GLuint frameSquarePosHandle;
      GLuint frameSquareElemHandle;   

   private:
      void initFBO();   
};

#endif
