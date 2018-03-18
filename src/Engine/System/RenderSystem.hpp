#pragma once
#ifndef _RENDER_SYSTEM_HPP_
#define _RENDER_SYSTEM_HPP_



#include <type_traits>
#include <typeindex>
#include <iostream>

#include "System.hpp"

#include "Shaders/DiffuseShader.hpp"
#include "Shaders/BounderShader.hpp"
#include "Shaders/OctreeShader.hpp"
#include "Shaders/RayShader.hpp"
#include "Shaders/PostProcessShader.hpp"
#include "Shaders/ShadowDepthShader.hpp"
#include "Shaders/BlurShader.hpp"



class DiffuseRenderComponent;



// static class
class RenderSystem {

    friend Scene;

public:

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
    static UniquePtr<BlurShader> s_blurShader;

    /* FBO Stuff */
    static GLuint getFBOTexture() { return s_fboColorTexs[0]; }

    static GLuint getBloomTexture() { return s_pingpongColorbuffers[0]; }

    static void getFrustumComps(const CameraComponent *, Vector<DiffuseRenderComponent *> &);

private:

    static void doBloom();

private:

    static const Vector<DiffuseRenderComponent *> & s_diffuseComponents;

    static void initFBO();
    static void doResize();
    static GLuint s_fbo;
    static GLuint s_fboColorTexs[2];
    static GLuint s_pingpongFBO[2];
    static GLuint s_pingpongColorbuffers[2];
    static bool s_wasResize;

};



#endif