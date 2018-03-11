#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"
#include "Scene/Scene.hpp"



const Vector<DiffuseRenderComponent *> & RenderSystem::s_diffuseComponents(Scene::getComponents<DiffuseRenderComponent>());
UnorderedMap<std::type_index, UniquePtr<Shader>> RenderSystem::s_shaders;
const CameraComponent * RenderSystem::s_camera = nullptr;

void RenderSystem::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);
    glViewport(0, 0, Window::getFrameSize().x, Window::getFrameSize().y);

    auto sizeCallback([&] (const Message & msg_) {
        const WindowFrameSizeMessage & msg(static_cast<const WindowFrameSizeMessage &>(msg_));
        glViewport(0, 0, msg.frameSize.x, msg.frameSize.y);
    });
    Scene::addReceiver<WindowFrameSizeMessage>(nullptr, sizeCallback);
}

///////////////////////////  TODO  ///////////////////////////
// pass a list of render components that are specific       //
// to this shader -- right now we are passing the entire    //
// list and expecting each shader to filter through         //
//////////////////////////////////////////////////////////////
void RenderSystem::update(float dt) {
    static UnorderedSet<GameObject *> s_visibleGOs;
    static Vector<Component *> s_compsToRender;
    static bool s_wasRender = true;

    if (s_wasRender) {
        /* Reset rendering display */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        s_wasRender = false;
    }

    if (s_camera) {
        // Frustum culling
        s_compsToRender.clear();
        s_visibleGOs.clear();
        CollisionSystem::getVisible(*s_camera, s_visibleGOs);
        for (GameObject * go : s_visibleGOs) {
            for (DiffuseRenderComponent * comp : go->getComponentsByType<DiffuseRenderComponent>()) {
                s_compsToRender.push_back(comp);
            }
        }

        /* Loop through active shaders */
        for (auto &shader : s_shaders) {
            if (!shader.second->isEnabled()) {
                continue;
            }

            shader.second->bind();
            // this reinterpret_cast business works because unique_ptr's data is
            // guaranteed is the same as a pointer
            shader.second->render(s_camera, reinterpret_cast<const Vector<Component *> &>(s_compsToRender));
            shader.second->unbind();

            s_wasRender = true;
        }
    }

    /* ImGui */
#ifdef DEBUG_MODE
    if (Window::isImGuiEnabled()) {
        ImGui::Render();
        s_wasRender = true;
    }
#endif
}

void RenderSystem::setCamera(const CameraComponent * camera) {
    s_camera = camera;
}