#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "Scene/Scene.hpp"

const Vector<DiffuseRenderComponent *> & RenderSystem::s_diffuseComponents(Scene::getComponents<DiffuseRenderComponent>());
UnorderedMap<std::type_index, UniquePtr<Shader>> RenderSystem::s_shaders;
UniquePtr<PostProcessShader> RenderSystem::s_postProcessShader;
const CameraComponent * RenderSystem::s_camera = nullptr;
GLuint RenderSystem::s_fbo = 0;
GLuint RenderSystem::s_fboColorTex = 0;
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

    //--------------------------------------------------------------------------
    // Shader Setup

    // Diffuse shader
    // TODO: temporary until jaafer's pr is merged
    glm::vec3 lightDir(glm::normalize(glm::vec3(1.0f)));
    if (!RenderSystem::createShader<DiffuseShader>("diffuse_vert.glsl", "diffuse_frag.glsl", lightDir)) {
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }

    // Bounder shader
    if (!RenderSystem::createShader<BounderShader>("bounder_vert.glsl", "bounder_frag.glsl")) {
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }

    // Octree shader
    if (!RenderSystem::createShader<OctreeShader>("bounder_vert.glsl", "bounder_frag.glsl")) {
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }
    
    // Ray shader
    if (!RenderSystem::createShader<RayShader>("ray_vert.glsl", "ray_frag.glsl")) {
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }

    // Setup square shader
    s_postProcessShader = UniquePtr<PostProcessShader>::make("postprocess_vert.glsl", "postprocess_frag.glsl");
    if (!s_postProcessShader->init()) {
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

    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        }
    }

    // Make it so that rendering is done to the computer screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset rendering display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    s_postProcessShader->bind();
    // The second parameter is passed by reference (not by pointer),
    // hence the funny pointer business
    s_postProcessShader->render(nullptr, *((Vector<Component *> *) nullptr));
    s_postProcessShader->unbind();
}

void RenderSystem::setCamera(const CameraComponent * camera) {
    s_camera = camera;
}

void RenderSystem::initFBO() {
    // Initialize framebuffer to draw into
    glGenFramebuffers(1, &s_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);

    glm::ivec2 size = Window::getFrameSize();

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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderSystem::doResize() {
    glViewport(0, 0, Window::getFrameSize().x, Window::getFrameSize().y);
    initFBO();
}