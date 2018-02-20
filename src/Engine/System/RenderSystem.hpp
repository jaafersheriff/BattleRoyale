#pragma once
#ifndef _RENDER_SYSTEM_HPP_
#define _RENDER_SYSTEM_HPP_



#include <unordered_map>
#include <string>
#include <type_traits>
#include <typeindex>
#include <iostream>

#include "System.hpp"

#include "Shaders/Shader.hpp"
#include "Shaders/DiffuseShader.hpp"
#include "Shaders/BounderShader.hpp"
#include "Component/RenderComponents/DiffuseRenderComponent.hpp"



// static class
class RenderSystem {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::render;

    static constexpr float k_defNear = 0.01f, k_defFar = 500.0f;

    public:

    static void init();

    /* Iterate through shaders map
        * Bind individual shaders 
        * Call shaders' render function with the appropriate render component list */
    static void update(float dt);

    // creates a new shader and initializes it
    template<typename ShaderT, typename... Args> static bool createShader(Args &&... args);

    // takes possession of shader and initializes it
    template <typename ShaderT> static bool addShader(std::unique_ptr<ShaderT> shader);

    // get shader of the specified type
    template <typename ShaderT> static ShaderT * getShader();

    static void setNearFar(float near, float far);

    static float near() { return s_near; }
    static float far() { return s_far; }

    static void setCamera(const CameraComponent * camera);

    private:
    
    static void add(std::unique_ptr<Component> component);

    static void remove(Component * component);
    
    private:

    static std::vector<std::unique_ptr<DiffuseRenderComponent>> s_diffuseComponents;
    static std::unordered_map<std::type_index, std::unique_ptr<Shader>> s_shaders;
    static float s_near, s_far;
    static const CameraComponent * s_camera;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////

template<typename ShaderT, typename... Args>
bool RenderSystem::createShader(Args &&... args) {
    return addShader(std::unique_ptr<ShaderT>(new ShaderT(std::forward<Args>(args)...)));
}

template <typename ShaderT>
bool RenderSystem::addShader(std::unique_ptr<ShaderT> shader) {
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
        std::cerr << "Failed to initialize shader" << std::endl;
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
