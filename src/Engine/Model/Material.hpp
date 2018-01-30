#pragma once
#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include "glm/glm.hpp"

class Material {
    public:
        /* Constructor */
        Material() :
            ambient(0.f),
            diffuse(glm::vec3(0.f)),
            specular(glm::vec3(0.f)),
            shineDamper(1.f)
        {}

        Material(const float a, const glm::vec3 d, const glm::vec3 s) :
            ambient(a),
            diffuse(d),
            specular(s),
            shineDamper(1.f)
        {}

        float ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float shineDamper;
        
};

#endif