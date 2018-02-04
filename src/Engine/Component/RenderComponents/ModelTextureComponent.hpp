#pragma once
#ifndef _MODEL_TEXTURE_COMPONENT_HPP_
#define _MODEL_TEXTURE_COMPONENT_HPP_

#include "Component/Component.hpp"

#include "Model/ModelTexture.hpp"

class ModelTextureComponent : public Component {
    public:
        ModelTextureComponent(Texture *t) :
            modelTexture(t)
        {}
        ModelTextureComponent(const float a, const glm::vec3 d, const glm::vec3 s) :
            modelTexture(a, d, s)
        {}
        ModelTextureComponent(Texture *t, const float a, const glm::vec3 d, const glm::vec3 s) :
            modelTexture(t, a, d, s)
        {}

        void init() {}
        void update(float) {}
        ModelTexture modelTexture;
};

#endif