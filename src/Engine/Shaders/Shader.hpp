/* Abstract parent Shader class 
 * Every feature will have its own derived shader */
#pragma once
#ifndef _SHADER_HPP_
#define _SHADER_HPP_

#include "glm/glm.hpp"

#include "Util/Memory.hpp"

class CameraComponent;
class DiffuseRenderComponent;

class Shader {
    public:
        /* Empty constructor
         * Only used to set GLSL shader names */
        Shader(const String & v = "", const String & f = "") : 
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
        virtual void render(const CameraComponent * camera) = 0;

        /* Parent load functions */
        void loadBool(const int, const bool) const;
        void loadInt(const int, const int) const;
        void loadFloat(const int, const float) const;
        void loadVec2(const int, const glm::vec2 &) const;
        void loadVec3(const int, const glm::vec3 &) const;
        void loadMat3(const int, const glm::mat3 &) const;
        void loadMat4(const int, const glm::mat4 &) const;
        void loadMultiMat4(const int, const glm::mat4 *, int) const;
        void loadMultiMat3(const int, const glm::mat3 *, int) const;

        /* Get shader location */
        int getAttribute(const String &);
        int getUniform(const String &);

         /* GLSL shader names */
        const String vShaderName;
        const String fShaderName;
        unsigned int pid = 0;

        bool isEnabled() const { return m_isEnabled; }
        void setEnabled(bool enabled) { m_isEnabled = enabled; }
        void toggleEnabled() { m_isEnabled = !m_isEnabled; } 
    protected:
        bool m_isEnabled;

        /* Shared GLSL utility functions */
        void addAttribute(const String &);
        void addUniform(const String &);
    private:    
        /* GLSL shader attributes */
        int vShaderId;
        int fShaderId;
        UnorderedMap<String, int> attributes;
        UnorderedMap<String, int> uniforms;
};

#endif