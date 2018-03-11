#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"
#include "Scene/Scene.hpp"
#include "EngineApp/EngineApp.hpp"


const Vector<DiffuseRenderComponent *> & RenderSystem::s_diffuseComponents(Scene::getComponents<DiffuseRenderComponent>());
const CameraComponent * RenderSystem::s_playerCamera = nullptr;
GameObject * RenderSystem::s_lightObject = nullptr;
CameraComponent * RenderSystem::s_lightCamera = nullptr;
SpatialComponent * RenderSystem::s_lightSpatial = nullptr;
ShadowDepthShader * RenderSystem::shadowShader = nullptr;
DiffuseShader * RenderSystem::diffuseShader = nullptr;
BounderShader * RenderSystem::bounderShader = nullptr;
RayShader * RenderSystem::rayShader = nullptr;
 
float RenderSystem::lightDist = 1.f;

void RenderSystem::init() {
    /* Init GL state */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Init light */
    s_lightObject = &Scene::createGameObject();
    s_lightCamera = &Scene::addComponent<CameraComponent>(*s_lightObject, 45.f, 0.01f, 300.f);
    glm::vec3 w = glm::normalize(glm::vec3(-0.2f, 0.75f, 0.5f));
    glm::vec3 u = glm::normalize(glm::cross(w, glm::vec3(0.f, 1.f, 0.f)));
    glm::vec3 v = glm::normalize(glm::cross(u, w));
    s_lightSpatial = &Scene::addComponent<SpatialComponent>(*s_lightObject, w * lightDist, glm::vec3(0.f), glm::mat3(u, v, w));
    s_lightCamera = &Scene::addComponent<CameraComponent>(*s_lightObject, 45.f, 0.01f, 300.f);

    /* Init shadow shader */
    shadowShader = new ShadowDepthShader(
        EngineApp::RESOURCE_DIR + "shadow_vert.glsl",
        EngineApp::RESOURCE_DIR + "shadow_frag.glsl",
        1280,
        960);
    if (!shadowShader->init()) {
        std::cerr << "Error initializing shadow shader" << std::endl;
        std::cin.get();
    }
}

void RenderSystem::update(float dt) {
    /* Update light */
    s_lightSpatial->setPosition(getLightDir() * -lightDist);

    /* Render to shadow map */
    //shadowShader->prepareRender(s_lightCamera);
    //renderScene(s_lightCamera, true);
    //shadowShader->finishRender();

    /* Regularly render scene */
    renderScene(s_playerCamera, false);

    /* Render ImGui */
#ifdef DEBUG_MODE
    if (Window::isImGuiEnabled()) {
        ImGui::Render();
    }
#endif
}

void RenderSystem::renderScene(const CameraComponent *camera, bool shadowRender) {
    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    glm::ivec2 size = Window::getFrameSize();
    glViewport(0, 0, size.x, size.y);
    if (!camera) {
        return;
    }
    
    /* Get components in frustum */
    // TODO : make this DiffuseRenderComponent
    Vector<Component *> s_compsToRender = getFrustumComps(camera);

    /* Render diffused */
    diffuseShader->bind();
    diffuseShader->render(camera, s_compsToRender);
    diffuseShader->unbind();
    /* Render ray */
    rayShader->bind();
    rayShader->render(camera, s_compsToRender);
    rayShader->unbind();
    /* Render bounder */
    bounderShader->bind();
    bounderShader->render(camera, s_compsToRender);
    bounderShader->unbind();
}

void RenderSystem::setCamera(const CameraComponent * camera) {
    s_playerCamera = camera;
}

glm::vec3 RenderSystem::getLightDir() {
    return s_lightCamera->getLookDir();
}

void RenderSystem::setLightDir(glm::vec3 in) {
    s_lightCamera->lookInDir(in);
}

bool RenderSystem::createDiffuseShader(String vert, String frag) {
    diffuseShader = new DiffuseShader(vert, frag);
    return initShader(diffuseShader);
}

bool RenderSystem::createBounderShader(String vert, String frag) {
    bounderShader = new BounderShader(vert, frag);
    return initShader(bounderShader);
}

bool RenderSystem::createRayShader(String vert, String frag) {
    rayShader = new RayShader(vert, frag);
    return initShader(rayShader);
}

bool RenderSystem::initShader(Shader *shader) {
    if (shader->init()) {
        return true;
    }

    std::cerr << "Failed to initialize shader:" << std::endl;
    std::cerr << "\t" << shader->vShaderName << std::endl;
    std::cerr << "\t" << shader->fShaderName << std::endl;
    std::cin.get();
    return false;
}

/* Frustum culling */
Vector<Component *> RenderSystem::getFrustumComps(const CameraComponent *camera) {
    Vector<Component *> renderComps;
    for (auto comp : s_diffuseComponents) {
        const Vector<Component *> & bounders(comp->gameObject().getComponentsByType<BounderComponent>());
        /* Case game object doesn't have bounder component */
        if (!bounders.size()) {
            renderComps.push_back(comp);
            continue;
        }
        /* Frustum cull through game object's bounders */
        for (Component * bounder_ : bounders) {
            BounderComponent * bounder(static_cast<BounderComponent *>(bounder_));
            if (camera->sphereInFrustum(bounder->enclosingSphere())) {
                renderComps.push_back(comp);
                break;
            }
        }
    }
    return renderComps;
}