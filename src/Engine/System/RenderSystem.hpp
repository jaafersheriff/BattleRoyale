#pragma once
#ifndef _RENDER_SYSTEM_HPP_
#define _RENDER_SYSTEM_HPP_



#include <unordered_map>
#include <string>
#include <type_traits>
#include <typeindex>

#include "System.hpp"

#include "Shaders/Shader.hpp"
#include "Shaders/DiffuseShader.hpp"
#include "Shaders/BounderShader.hpp"

#include <iostream>

class RenderSystem : public System {

    friend Scene;

    private: // only scene can create system

    RenderSystem(std::vector<Component *> & components);

    public:

    // creates a new shader and initializes it
    template<typename ShaderT, typename... Args> bool createShader(Args &&... args);

    // takes possession of shader and initializes it
    template <typename ShaderT> bool addShader(std::unique_ptr<ShaderT> shader);

    // get shader of the specified type
    template <typename ShaderT> ShaderT * getShader();
    template <typename ShaderT> const Shader * getShader() const {
        return const_cast<RenderSystem *>(this)->getShader<ShaderT>();
    }

    /* Iterate through shaders map
        * Bind individual shaders 
        * Call shaders' render function with the appropriate render component list */
    void update(float);
    
    // Map of shader type to Shader objects
    std::unordered_map<std::type_index, std::unique_ptr<Shader>> m_shaders;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////

template<typename ShaderT, typename... Args>
bool RenderSystem::createShader(Args &&... args) {
    return addShader(std::unique_ptr<ShaderT>(new ShaderT(std::forward<Args>(args)...)));
}

template <typename ShaderT>
bool RenderSystem::addShader(std::unique_ptr<ShaderT> shader) {
    std::type_index typeI(typeid(ShaderT));
    auto it(m_shaders.find(typeI));
    if (it != m_shaders.end()) {
        return true;
    }
    if (shader->init()) {
        m_shaders[typeI] = std::move(shader);
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
    if (!m_shaders.count(typeI)) {
        return nullptr;
    }
    return static_cast<ShaderT *>(m_shaders.at(typeI).get());
}



#endif
