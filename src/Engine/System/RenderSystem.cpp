#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "Scene/Scene.hpp"



std::vector<DiffuseRenderComponent *> RenderSystem::s_diffuseComponents;
std::unordered_map<std::type_index, std::unique_ptr<Shader>> RenderSystem::s_shaders;
float RenderSystem::s_near = k_defNear, RenderSystem::s_far = k_defFar;
const CameraComponent * RenderSystem::s_camera = nullptr;

void RenderSystem::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderSystem::update(float dt) {
    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    if (s_camera) {
        /* Loop through active shaders */
        for (auto &shader : s_shaders) {
            if (!shader.second->isEnabled()) {
                continue;
            }

            shader.second->bind();
            ///////////////////////////  TODO  ///////////////////////////
            // pass a list of render components that are specific       //
            // to this shader -- right now we are passing the entire    //
            // list and expecting each shader to filter through         //
            //////////////////////////////////////////////////////////////

            // this reinterpret_cast business works because unique_ptr's data is
            // guaranteed is the same as a pointer
            shader.second->render(*s_camera, reinterpret_cast<std::vector<Component *> &>(s_diffuseComponents));
            shader.second->unbind();
        }
    }

    /* ImGui */
    if (Window::isImGuiEnabled()) {
        ImGui::Render();
    }
}

void RenderSystem::add(Component & component) {
    if (dynamic_cast<DiffuseRenderComponent *>(&component))
        s_diffuseComponents.emplace_back(static_cast<DiffuseRenderComponent *>(&component));
    else
        assert(false);
}

void RenderSystem::remove(Component & component) {
    if (dynamic_cast<DiffuseRenderComponent *>(&component)) {
        for (auto it(s_diffuseComponents.begin()); it != s_diffuseComponents.end(); ++it) {
            if (*it == &component) {
                s_diffuseComponents.erase(it);
                break;
            }
        }
    }
}

void RenderSystem::setNearFar(float near, float far) {
    s_near = near;
    s_far = far;
    Scene::sendMessage<NearFarMessage>(nullptr, near, far);
}

void RenderSystem::setCamera(const CameraComponent * camera) {
    s_camera = camera;
}