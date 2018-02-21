/* Abstract parent Shader class 
 * Every feature will have its own derived shader */
#pragma once
#ifndef _SHADER_HPP_
#define _SHADER_HPP_

#define GLEW_STATIC
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <map>
#include <string>
#include <vector>

class Component;
class CameraComponent;


class Shader {
    public:
        /* Empty constructor
         * Only used to set GLSL shader names */
        Shader(const std::string & v = "", const std::string & f = "") : 
            vShaderName(v), 
            fShaderName(f),
            m_isEnabled(false)
        {}

        /* Compile GLSL shaders 
         * Derived shaders can add uniforms/attributes here */  
        virtual bool init();

        /* Utility functions */
        void bind();
        void unbind();
        void cleanUp();
        void unloadMesh();
        void unloadTexture(int);

        /* Render functions */
        virtual void render(const CameraComponent & camera, const std::vector<Component *> & components) = 0;

        /* Parent load functions */
        void loadBool(const int, const bool) const;
        void loadInt(const int, const int) const;
        void loadFloat(const int, const float) const;
        void loadVec2(const int, const glm::vec2 &) const;
        void loadVec3(const int, const glm::vec3 &) const;
        void loadMat3(const int, const glm::mat3 &) const;
        void loadMat4(const int, const glm::mat4 &) const;

        /* Get shader location */
        GLint getAttribute(const std::string &);
        GLint getUniform(const std::string &);

        GLuint pid = 0;

        bool isEnabled() const { return m_isEnabled; }
        void toggleEnabled() { m_isEnabled = !m_isEnabled; } 
    protected:
        /* GLSL shader names */
        const std::string vShaderName;
        const std::string fShaderName;

        bool m_isEnabled;

        /* Shared GLSL utility functions */
        void addAttribute(const std::string &);
        void addUniform(const std::string &);
    private:    
        /* GLSL shader attributes */
        GLint vShaderId;
        GLint fShaderId;
        std::map<std::string, GLint> attributes;
        std::map<std::string, GLint> uniforms;
};

#endif