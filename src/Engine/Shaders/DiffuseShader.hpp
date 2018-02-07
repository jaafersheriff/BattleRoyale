/* Basic diffuse shader */
#pragma once
#ifndef _DIFFUSE_SHADER_HPP_
#define _DIFFUSE_SHADER_HPP_

#include "Shader.hpp"

class DiffuseShader : public Shader {
    public:
        DiffuseShader(std::string vert, std::string frag) :
            Shader(vert, frag)
        {}

        void render();
};

#endif 