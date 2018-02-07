#pragma once
#ifndef _RENDER_SYSTEM_HPP_
#define _RENDER_SYSTEM_HPP_

#include "System.hpp"

#include "Shaders/Shader.hpp"
#include "Shaders/DiffuseShader.hpp"

#include <map>
#include <string>

class RenderSystem : public System {
    public:
        RenderSystem(std::vector<Component *> *);

        /* If the shader already exists, return it
         * Otherwise, initialize shader object
         *   Compile GLSL shaders
         *   On success, add to shader map and return true */
        template<class T, class... Args>
        void addShader(std::string name, std::string vertex, std::string fragment, Args&&... args) {
            auto it = shaders.find(name);
            if (it != shaders.end()) {
                return;
            }
            T* shader = new T(vertex, fragment, args...);
            if (shader->init()) {
                shaders.insert(std::map<std::string, Shader *>::value_type(name, shader));
            }
        }

        /* Iterate through shaders map
         * Bind individual shaders 
         * Call shaders' render function with the appropriate renderable component list */
        void update(float);
    
        /* Map of shader name to Shader objects 
         * Rendering components only need to contain a reference to the 
         * Shader name string -- the render system will handle the rest */
        std::map<std::string, Shader *> shaders;
};

#endif
