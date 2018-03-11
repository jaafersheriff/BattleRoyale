#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"
#include "Scene/Scene.hpp"

const Vector<DiffuseRenderComponent *> & RenderSystem::s_diffuseComponents(Scene::getComponents<DiffuseRenderComponent>());
UnorderedMap<std::type_index, UniquePtr<Shader>> RenderSystem::s_shaders;
const CameraComponent * RenderSystem::s_camera = nullptr;
// SquareShader *RenderSystem::squareShader = new SquareShader("square_vert.glsl", "square_frag.glsl");
std::unique_ptr<SquareShader> RenderSystem::squareShader(new SquareShader("square_vert.glsl", "square_frag.glsl"));

void RenderSystem::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    squareShader->init();
}

///////////////////////////  TODO  ///////////////////////////
// pass a list of render components that are specific       //
// to this shader -- right now we are passing the entire    //
// list and expecting each shader to filter through         //
//////////////////////////////////////////////////////////////
void RenderSystem::update(float dt) {
    // Make it so that rendering is not done to the computer screen
    // but to the framebuffer in squareShader->fboHandle
    glBindFramebuffer(GL_FRAMEBUFFER, squareShader->fboHandle);

    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    glm::ivec2 size = Window::getFrameSize();
    glViewport(0, 0, size.x, size.y);
    if (s_camera) {
        /* Loop through active shaders */
        for (auto &shader : s_shaders) {
            if (!shader.second->isEnabled()) {
                continue;
            }

            /* Frustum culling */
            static Vector<Component *> s_compsToRender;
            for (auto comp : s_diffuseComponents) {
                const Vector<Component *> & bounders(comp->gameObject().getComponentsByType<BounderComponent>());
                if (bounders.size()) {
                    bool inFrustum(false);
                    for (Component * bounder_ : bounders) {
                        BounderComponent * bounder(static_cast<BounderComponent *>(bounder_));
                        if (s_camera->sphereInFrustum(bounder->enclosingSphere())) {
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

            shader.second->bind();
            // this reinterpret_cast business works because unique_ptr's data is
            // guaranteed is the same as a pointer
            shader.second->render(s_camera, reinterpret_cast<const Vector<Component *> &>(s_compsToRender));
            shader.second->unbind();

            s_compsToRender.clear();
        }
    }

    /* ImGui */
#ifdef DEBUG_MODE
    if (Window::isImGuiEnabled()) {
        ImGui::Render();
    }
#endif

    // Make it so that rendering is done to the computer screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset rendering display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    glViewport(0, 0, size.x, size.y);

    squareShader->bind();
    // The second parameter is passed by reference (not by pointer),
    // hence the funny pointer business
    squareShader->render(nullptr, *((Vector<Component *> *) nullptr));
    squareShader->unbind();
}

void RenderSystem::setCamera(const CameraComponent * camera) {
    s_camera = camera;
}