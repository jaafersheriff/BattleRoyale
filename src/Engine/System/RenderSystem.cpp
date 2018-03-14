#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"
#include "Scene/Scene.hpp"

const Vector<DiffuseRenderComponent *> & RenderSystem::s_diffuseComponents(Scene::getComponents<DiffuseRenderComponent>());
UnorderedMap<std::type_index, UniquePtr<Shader>> RenderSystem::s_shaders;
UniquePtr<PostProcessShader> RenderSystem::s_postProcessShader;
UniquePtr<BlurShader> RenderSystem::s_BlurShader;
const CameraComponent * RenderSystem::s_camera = nullptr;
GLuint RenderSystem::s_fbo = 0;
GLuint RenderSystem::s_fboColorTex = 0;
GLuint RenderSystem::colorBuffers[2];
GLuint RenderSystem::pingpongFBO[2];
GLuint RenderSystem::pingpongColorbuffers[2];
bool RenderSystem::s_wasResize = false;

void RenderSystem::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    auto sizeCallback([&] (const Message & msg_) {
        const WindowFrameSizeMessage & msg(static_cast<const WindowFrameSizeMessage &>(msg_));
        s_wasResize = true;
    });
    Scene::addReceiver<WindowFrameSizeMessage>(nullptr, sizeCallback);    

    // Setup square shader
    s_postProcessShader = UniquePtr<PostProcessShader>::make("postprocess_vert.glsl", "postprocess_frag.glsl");
    if (!s_postProcessShader->init()) {
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }

    //Setup gaussian blur shader
    s_BlurShader = UniquePtr<BlurShader>::make("pass_vert.glsl", "blur_frag.glsl");
    if (!s_BlurShader->init()) {
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }
    
    glViewport(0, 0, Window::getFrameSize().x, Window::getFrameSize().y);
    initFBO();
}

///////////////////////////  TODO  ///////////////////////////
// pass a list of render components that are specific       //
// to this shader -- right now we are passing the entire    //
// list and expecting each shader to filter through         //
//////////////////////////////////////////////////////////////
void RenderSystem::update(float dt) {
    if (s_wasResize) {
        doResize();
        s_wasResize = false;
    }

    // Make it so that rendering is not done to the computer screen
    // but to the framebuffer in squareShader->fboHandle
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);

    static Vector<Component *> s_compsToRender;
    static bool s_wasRender = true;

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

    //Blur bright frags
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Reset rendering display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    
    s_BlurShader->bind();
    for (unsigned int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        s_BlurShader->s_Horizontal = horizontal;
        //glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]); // bind texture of other framebuffer (or scene if first iteration)
        s_BlurShader->render(nullptr, *((Vector<Component *> *) nullptr));
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    s_BlurShader->unbind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    s_postProcessShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    //glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
    s_postProcessShader->render(nullptr, *((Vector<Component *> *) nullptr));
    s_postProcessShader->unbind();

    /*
    s_postProcessShader->bind();
    // The second parameter is passed by reference (not by pointer),
    // hence the funny pointer business

    s_postProcessShader->render(nullptr, *((Vector<Component *> *) nullptr));
    s_postProcessShader->unbind();
    */


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

void RenderSystem::initFBO() {
    // Initialize framebuffer to draw into
    glGenFramebuffers(1, &s_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);

    glm::ivec2 size = Window::getFrameSize();

    //Create 2 color buffers
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB16F, size.x, size.y, 0, GL_RGB, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
        );
    }

    /*
    // Set the first texture unit as active
    glActiveTexture(GL_TEXTURE0);

    // Attach color to the framebuffer
    glGenTextures(1, &s_fboColorTex);
    glBindTexture(GL_TEXTURE_2D, s_fboColorTex);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_fboColorTex, 0
    );
    */

    //Create depth and stencil buffers
    RenderSystem::CreateRenderBuffer(size);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // ping-pong-framebuffer for blurring
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, size.x, size.y, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }
}

void RenderSystem::CreateRenderBuffer(glm::ivec2 size) {
    // Create Renderbuffer Object to hold depth and stencil buffers
    GLuint fboDepthRB(0);
    glGenRenderbuffers(1, &fboDepthRB);
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepthRB);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepthRB);

    // Using a texture instead
    //glGenTextures(1, &depthTexture);
    //glBindTexture(GL_TEXTURE_2D, depthTexture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
}

void RenderSystem::doResize() {
    glViewport(0, 0, Window::getFrameSize().x, Window::getFrameSize().y);
    initFBO();
}