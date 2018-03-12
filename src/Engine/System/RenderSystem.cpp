#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"
#include "Scene/Scene.hpp"

const Vector<DiffuseRenderComponent *> & RenderSystem::s_diffuseComponents(Scene::getComponents<DiffuseRenderComponent>());
UnorderedMap<std::type_index, UniquePtr<Shader>> RenderSystem::s_shaders;
const CameraComponent * RenderSystem::s_camera = nullptr;
UniquePtr<SquareShader> RenderSystem::squareShader(
    UniquePtr<SquareShader>::make("square_vert.glsl", "square_frag.glsl")
);

void RenderSystem::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);
    // Viewport begin
    glViewport(0, 0, Window::getFrameSize().x, Window::getFrameSize().y);

    auto sizeCallback([&] (const Message & msg_) {
        const WindowFrameSizeMessage & msg(static_cast<const WindowFrameSizeMessage &>(msg_));
        glViewport(0, 0, msg.frameSize.x, msg.frameSize.y);
    });
    Scene::addReceiver<WindowFrameSizeMessage>(nullptr, sizeCallback);
    // Viewport end

    if (!squareShader->init()) {
        std::cerr << "Failed to initialize shader:" << std::endl;
        std::cerr << "\t" << squareShader->vShaderName << std::endl;
        std::cerr << "\t" << squareShader->fShaderName << std::endl;
        std::cin.get();
    }

    /*
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!squareShader->init()) {
        std::cerr << "Failed to initialize shader:" << std::endl;
        std::cerr << "\t" << squareShader->vShaderName << std::endl;
        std::cerr << "\t" << squareShader->fShaderName << std::endl;
        std::cin.get();
    }
    */
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

    static Vector<Component *> s_compsToRender;
    static bool s_wasRender = true;

    glm::ivec2 size = Window::getFrameSize();
    //glViewport(0, 0, size.x, size.y);
    //glViewport(0, 0, viewport.x, viewport.y);

    // Update components
    for (DiffuseRenderComponent * comp : s_diffuseComponents) {
        comp->update(dt);
    }
    // Frustum culling
    s_compsToRender.clear();
    if (s_camera) {
        for (DiffuseRenderComponent * comp : s_diffuseComponents) {
            if (s_camera->sphereInFrustum(comp->enclosingSphere())) {
                s_compsToRender.push_back(comp);
            }
        }
    }

    if (s_wasRender) {
        /* Reset rendering display */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        s_wasRender = false;
    }

    if (s_camera) {

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

    // Make it so that rendering is done to the computer screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset rendering display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glViewport(0, 0, viewport.x, viewport.y);
    //glViewport(0, 0, size.x, size.y);
    // glViewport(0, 0, 400, 200);
    // glViewport(0, 0, Window::getFrameSize().x, Window::getFrameSize().y);

    squareShader->bind();
    // The second parameter is passed by reference (not by pointer),
    // hence the funny pointer business
    squareShader->render(nullptr, *((Vector<Component *> *) nullptr));
    squareShader->unbind();

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