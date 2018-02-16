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



// Singleton
class RenderSystem : public System {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::render;

    static constexpr float k_defNear = 0.01f, k_defFar = 250.0f;

    public:

    static RenderSystem & get() {
        static RenderSystem s_renderSystem;
        return s_renderSystem;
    }

    private:

    RenderSystem() = default;

    public:

    virtual void init() override;

    /* Iterate through shaders map
        * Bind individual shaders 
        * Call shaders' render function with the appropriate render component list */
    virtual void update(float dt) override;

    // creates a new shader and initializes it
    template<typename ShaderT, typename... Args> bool createShader(Args &&... args);

    // takes possession of shader and initializes it
    template <typename ShaderT> bool addShader(std::unique_ptr<ShaderT> shader);

    // get shader of the specified type
    template <typename ShaderT> ShaderT * getShader();
    template <typename ShaderT> const Shader * getShader() const {
        return const_cast<RenderSystem *>(this)->getShader<ShaderT>();
    }

    void setNearFar(float near, float far);

    float near() const { return m_near; }
    float far() const { return m_far; }

    void setCamera(const CameraComponent * camera);

    private:
    
    virtual void add(std::unique_ptr<Component> component) override;

    virtual void remove(Component * component) override;
    
    private:

    std::vector<std::unique_ptr<DiffuseRenderComponent>> m_diffuseComponents;
    std::unordered_map<std::type_index, std::unique_ptr<Shader>> m_shaders;

    float m_near = k_defNear, m_far = k_defFar;
    const CameraComponent * m_camera = nullptr;

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
