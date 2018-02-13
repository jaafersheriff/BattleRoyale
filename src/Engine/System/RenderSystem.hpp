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



class RenderSystem : public System {

    public:

    RenderSystem(std::vector<Component *> & components);

    /* If the shader already exists, return it
        * Otherwise, initialize shader object
        *   Compile GLSL shaders
        *   On success, add to shader map and return true */
    template<typename ShaderT, typename... Args>
    bool addShader(const std::string & name, Args &&... args) {
        return addShader(name, std::unique_ptr<T>(new ShaderT(std::forward<Args>(args)...)));
    }
        
    /* If the shader already exists, return it
        * Otherwise, initialize shader object
        *   Compile GLSL shaders
        *   On success, add to shader map and return true */
    template <typename ShaderT>
    bool addShader(std::unique_ptr<ShaderT> shader) {
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

    /* Iterate through shaders map
        * Bind individual shaders 
        * Call shaders' render function with the appropriate render component list */
    void update(float);
    
    /* Map of shader name to Shader objects 
        * Rendering components only need to contain a reference to the 
        * Shader name string -- the render system will handle the rest */
    std::unordered_map<std::type_index, std::unique_ptr<Shader>> m_shaders;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



#endif
