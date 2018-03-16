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
#include "Shaders/OctreeShader.hpp"
#include "Shaders/RayShader.hpp"
#include "Shaders/PostProcessShader.hpp"
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

    /* Camera */
    static void setCamera(const CameraComponent * camera);
    static const CameraComponent * s_playerCamera;

    /* Light */
    static GameObject * s_lightObject;
    static SpatialComponent * s_lightSpatial;
    static CameraComponent * s_lightCamera;
    static glm::vec3 getLightDir();
    static void setLightDir(glm::vec3);
    static float lightDist;

    /* Shadows */
    static const glm::mat4 & getL() { return s_shadowShader->getL(); }
    static const GLuint getShadowMap() { return s_shadowShader->getShadowMapTexture(); }

    /* Shaders */
    static UniquePtr<DiffuseShader> s_diffuseShader;
    static UniquePtr<BounderShader> s_bounderShader;
    static UniquePtr<RayShader> s_rayShader;
    static UniquePtr<ShadowDepthShader> s_shadowShader;
    static UniquePtr<OctreeShader> s_octreeShader;
    static UniquePtr<PostProcessShader> s_postProcessShader;

    /* FBO Stuff */
    static GLuint getFBOTexture() { return s_fboColorTex; }

    static void getFrustumComps(const CameraComponent *, Vector<DiffuseRenderComponent *> &);

private:

    static const Vector<DiffuseRenderComponent *> & s_diffuseComponents;

    static void initFBO();
    static void doResize();
    static GLuint s_fbo;
    static GLuint s_fboColorTex;
    static bool s_wasResize;

};



#endif