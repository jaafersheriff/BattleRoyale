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
#include "Shaders/RayShader.hpp"
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

    /* Iterate through shaders map
        * Bind individual shaders 
        * Call shaders' render function with the appropriate render component list */
    static void renderScene(const CameraComponent *, bool shadowRender);

    /* Camera stuff */
    static void setCamera(const CameraComponent * camera);
    static const CameraComponent * s_playerCamera;

    /* Light stuff */
    static GameObject * s_lightObject;
    static SpatialComponent * s_lightSpatial;
    static CameraComponent * s_lightCamera;
    static glm::vec3 getLightDir();
    static void setLightDir(glm::vec3);
    static float lightDist;

    /* Shaders */
    static ShadowDepthShader * shadowShader;
    static DiffuseShader * diffuseShader;
    static bool createDiffuseShader(String, String);
    static BounderShader * bounderShader;
    static bool createBounderShader(String, String);
    static RayShader * rayShader;
    static bool createRayShader(String, String);

private:
    static bool initShader(Shader *);
    static Vector<DiffuseRenderComponent *> getFrustumComps(const CameraComponent *);

    /* Render targets */
    static const Vector<DiffuseRenderComponent *> & s_diffuseComponents;
};

#endif