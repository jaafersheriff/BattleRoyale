#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"
#include "Scene/Scene.hpp"
#include "EngineApp/EngineApp.hpp"


const Vector<DiffuseRenderComponent *> & RenderSystem::s_diffuseComponents(Scene::getComponents<DiffuseRenderComponent>());
UnorderedMap<std::type_index, UniquePtr<Shader>> RenderSystem::s_shaders;
const CameraComponent * RenderSystem::s_playerCamera = nullptr;
CameraComponent * RenderSystem::s_lightCamera = nullptr;
ShadowDepthShader * RenderSystem::shadowShader = nullptr;

void RenderSystem::init() {
    /* Init GL state */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Init light */
    

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

///////////////////////////  TODO  ///////////////////////////
// pass a list of render components that are specific       //
// to this shader -- right now we are passing the entire    //
// list and expecting each shader to filter through         //
//////////////////////////////////////////////////////////////
void RenderSystem::update(float dt) {
    /* Render to shadow map */
    shadowShader->prepareRender(getLightDir());
    renderScene(s_lightCamera, true);
    shadowShader->finishRender();

    /* Regularly render scene */
    renderScene(s_playerCamera, false);
}

void RenderSystem::renderScene(const CameraComponent *camera, bool shadowRender) {
    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    glm::ivec2 size = Window::getFrameSize();
    glViewport(0, 0, size.x, size.y);
    if (camera) {
        /* Loop through active shaders */
        for (auto &shader : s_shaders) {
            if (!shader.second->isEnabled()) {
                continue;
            }

            /* Frustum culling */
            static Vector<Component *> s_compsToRender;
            // TODO : call this on shader-specific component list
            for (auto comp : s_diffuseComponents) {
                const Vector<Component *> & bounders(comp->gameObject().getComponentsByType<BounderComponent>());
                if (bounders.size()) {
                    bool inFrustum(false);
                    for (Component * bounder_ : bounders) {
                        BounderComponent * bounder(static_cast<BounderComponent *>(bounder_));
                        if (camera->sphereInFrustum(bounder->enclosingSphere())) {
                            inFrustum = true;
                            break;
                        }
                    }
                    if (inFrustum) {
                        s_compsToRender.push_back(comp);
                    }
                }
                else {
                    s_compsToRender.push_back(comp);
                }
            }

            if (!shadowRender) {
                shader.second->bind();
            }
            // this reinterpret_cast business works because unique_ptr's data is
            // guaranteed is the same as a pointer
            shader.second->render(camera, reinterpret_cast<const Vector<Component *> &>(s_compsToRender));
            if (!shadowRender) {
                shader.second->unbind();
            }

            s_compsToRender.clear();
        }
    }

    /* ImGui */
#ifdef DEBUG_MODE
    if (Window::isImGuiEnabled()) {
        ImGui::Render();
    }
#endif
}

void RenderSystem::setCamera(const CameraComponent * camera) {
    s_playerCamera = camera;
}

void RenderSystem::setLightDir(const glm::vec3 dir) {
    s_lightCamera->setUVW(s_lightCamera->u(), s_lightCamera->v(), dir);
}

glm::vec3 RenderSystem::getLightDir() {
    return -s_lightCamera->getLookDir();
}
