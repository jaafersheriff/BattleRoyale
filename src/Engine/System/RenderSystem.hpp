#pragma once
#ifndef _RENDER_SYSTEM_HPP_
#define _RENDER_SYSTEM_HPP_



#include <type_traits>
#include <typeindex>
#include <iostream>

#include "System.hpp"

#include "Shaders/Shader.hpp"
#include "Shaders/DiffuseShader.hpp"
#include "Shaders/BounderShader.hpp"
#include "Shaders/RayShader.hpp"
#include "Shaders/ShadowDepthShader.hpp"
#include "Component/RenderComponents/DiffuseRenderComponent.hpp"



// static class
class RenderSystem {

    friend Scene;

public:
    static constexpr SystemID ID = SystemID::render;

    static void init();

    /* Full render function including shadow maps, main render calls, and post-processing */
    static void update(float dt);

    /* Iterate through shaders map
        * Bind individual shaders 
        * Call shaders' render function with the appropriate render component list */
    static void renderScene(const CameraComponent *, bool shadowRender);

    // creates a new shader and initializes it
    template<typename ShaderT, typename... Args> static bool createShader(Args &&... args);

    // takes possession of shader and initializes it
    template <typename ShaderT> static bool addShader(UniquePtr<ShaderT> shader);

    // get shader of the specified type
    template <typename ShaderT> static ShaderT * getShader();

    static void setCamera(const CameraComponent * camera);

    static const Vector<DiffuseRenderComponent *> & s_diffuseComponents;
    static UnorderedMap<std::type_index, UniquePtr<Shader>> s_shaders;

    static const CameraComponent * s_playerCamera;

    /* Light stuff */
    static GameObject * s_lightObject;
    static SpatialComponent * s_lightSpatial;
    static CameraComponent * s_lightCamera;
    static glm::vec3 getLightDir();
    static void setLightDir(glm::vec3);


    /* Shadow shader */
    static ShadowDepthShader * shadowShader;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////

template<typename ShaderT, typename... Args>
bool RenderSystem::createShader(Args &&... args) {
    auto shader(UniquePtr<ShaderT>::make(std::forward<Args>(args)...));
    std::type_index typeI(typeid(ShaderT));

    auto it(s_shaders.find(typeI));
    if (it != s_shaders.end()) {
        return true;
    }
    if (shader->init()) {
        s_shaders[typeI] = std::move(shader);
        return true;
    }
    else {
        std::cerr << "Failed to initialize shader:" << std::endl;
        std::cerr << "\t" << shader->vShaderName << std::endl;
        std::cerr << "\t" << shader->fShaderName << std::endl;
        std::cin.get();
        return false;
    }
}

template <typename ShaderT>
ShaderT * RenderSystem::getShader() {
    std::type_index typeI(typeid(ShaderT));
    if (!s_shaders.count(typeI)) {
        return nullptr;
    }
    return static_cast<ShaderT *>(s_shaders.at(typeI).get());
}



#endif
