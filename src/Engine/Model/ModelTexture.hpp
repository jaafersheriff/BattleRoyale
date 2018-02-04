/* Material class
 * Contains reference to an optionally loaded texture, texture properties, and material properties
 * Rendering will based on material properties if loaded texture doesn't exist */
#pragma once
#ifndef _MODEL_TEXTURE_HPP_
#define _MODEL_TEXTURE_HPP_

#include "Texture.hpp"
#include "Material.hpp"

#include "glm/glm.hpp"

class ModelTexture {
    public:
        /* Constructors */
        ModelTexture() :
            texture(nullptr),
            material(0.f, glm::vec3(0.f), glm::vec3(0.f))
        {}
        ModelTexture(Texture *texture) :
            texture(texture),
            material(0.f, glm::vec3(0.f), glm::vec3(1.f))
        {}
        ModelTexture(const float ambient, const glm::vec3 diffuse, const glm::vec3 specular) :
            texture(nullptr),
            material(ambient, diffuse, specular)
        {}
        ModelTexture(Texture *texture, const float ambient, const glm::vec3 diffuse, const glm::vec3 specular) :
            texture(texture),
            material(ambient, diffuse, specular)
        {}

        /* Texture properties */
        Texture *texture;

        /* Material */
        Material material;
};

#endif