#pragma once
#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include "glm/glm.hpp"

class Material {
    public:
        /* Constructor */
        Material() :
            diffuse(glm::vec3(0.5f)),
            specular(glm::vec3(0.5f)),
            shine(16.0f)
        {}

        Material(const glm::vec3 & diffuse, const glm::vec3 & specular, float shine) :
            diffuse(diffuse),
            specular(specular),
            shine(shine)
        {}

        glm::vec3 diffuse;
        glm::vec3 specular;
        float shine;
        
};

#endif