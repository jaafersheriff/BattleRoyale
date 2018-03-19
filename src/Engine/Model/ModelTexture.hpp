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
            material()
        {}
        ModelTexture(const Material & material) :
            texture(nullptr),
            material(material)
        {}
        ModelTexture(const Texture * texture) :
            texture(texture),
            material()
        {}
        ModelTexture(const Texture * texture, const Material & material) :
            texture(texture),
            material(material)
        {}

        /* Texture properties */
        const Texture * texture;

        /* Material */
        Material material;

};

#endif