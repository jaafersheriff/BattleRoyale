#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"
#include "Scene/Scene.hpp"



const Vector<DiffuseRenderComponent *> & RenderSystem::s_diffuseComponents(Scene::getComponents<DiffuseRenderComponent>());
UnorderedMap<std::type_index, UniquePtr<Shader>> RenderSystem::s_shaders;
const CameraComponent * RenderSystem::s_camera = nullptr;
GLuint RenderSystem::frameBuffer = 0;
GLuint RenderSystem::texColorBuffer = 0;
GLuint RenderSystem::texDepthBuffer = 0;
const Vector<float> RenderSystem::frameSquare = {
    1.f, 1.f,
    1.f, -1.f,
    -1.f, 1.f,
    -1.f, -1.f
};

void RenderSystem::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize texture to draw into
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glm::ivec2 size = Window::getFrameSize();

    // Attach color to the framebuffer
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0
    );

    // Attach depth to the framebuffer
    glGenTextures(1, &texDepthBuffer);
    glBindTexture(GL_TEXTURE_2D, texDepthBuffer);

    // TODO: Pack it tighter ???
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texDepthBuffer, 0
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

///////////////////////////  TODO  ///////////////////////////
// pass a list of render components that are specific       //
// to this shader -- right now we are passing the entire    //
// list and expecting each shader to filter through         //
//////////////////////////////////////////////////////////////
void RenderSystem::update(float dt) {
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
}

void RenderSystem::setCamera(const CameraComponent * camera) {
    s_camera = camera;
}