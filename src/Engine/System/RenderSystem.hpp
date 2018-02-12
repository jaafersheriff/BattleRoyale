#pragma once
#ifndef _RENDER_SYSTEM_HPP_
#define _RENDER_SYSTEM_HPP_

#include "System.hpp"

#include "Shaders/Shader.hpp"
#include "Shaders/DiffuseShader.hpp"

#include <unordered_map>
#include <string>

class RenderSystem : public System {

    public:

        RenderSystem(std::vector<Component *> & components);

        /* If the shader already exists, return it
         * Otherwise, initialize shader object
         *   Compile GLSL shaders
         *   On success, add to shader map and return true */
        template<typename T, typename... Args>
        bool addShader(const std::string & name, Args &&... args) {
            return addShader(name, std::unique_ptr<T>(new T(std::forward<Args>(args)...)));
        }
        
        /* If the shader already exists, return it
         * Otherwise, initialize shader object
         *   Compile GLSL shaders
         *   On success, add to shader map and return true */
        bool addShader(const std::string & name, std::unique_ptr<Shader> shader);

        /* Iterate through shaders map
         * Bind individual shaders 
         * Call shaders' render function with the appropriate render component list */
        void update(float);
    
        /* Map of shader name to Shader objects 
         * Rendering components only need to contain a reference to the 
         * Shader name string -- the render system will handle the rest */
        std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;

};

#endif
