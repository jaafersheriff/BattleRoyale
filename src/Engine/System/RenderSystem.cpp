#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "Scene/Scene.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/RenderComponents/DiffuseRenderComponent.hpp"

const Vector<DiffuseRenderComponent *> & RenderSystem::s_diffuseComponents(Scene::getComponents<DiffuseRenderComponent>());
/* FBO */
GLuint RenderSystem::s_fbo = 0;
GLuint RenderSystem::s_fboColorTexs[2];
GLuint RenderSystem::s_pingpongFBO[2];
GLuint RenderSystem::s_pingpongColorbuffers[2];
bool RenderSystem::s_wasResize = false;
/* Camera and light */
const CameraComponent * RenderSystem::s_playerCamera = nullptr;
GameObject * RenderSystem::s_lightObject = nullptr;
CameraComponent * RenderSystem::s_lightCamera = nullptr;
SpatialComponent * RenderSystem::s_lightSpatial = nullptr;
float RenderSystem::lightDist = 15.f;

/* Shaders */
UniquePtr<ShadowDepthShader> RenderSystem::s_shadowShader;;
UniquePtr<DiffuseShader> RenderSystem::s_diffuseShader;
UniquePtr<BounderShader> RenderSystem::s_bounderShader;
UniquePtr<RayShader> RenderSystem::s_rayShader;
UniquePtr<OctreeShader> RenderSystem::s_octreeShader;
UniquePtr<PostProcessShader> RenderSystem::s_postProcessShader;
UniquePtr<BlurShader> RenderSystem::s_blurShader;
 

void RenderSystem::init() {
    /* Init GL state */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    /* Init GL window */
    glViewport(0, 0, Window::getFrameSize().x, Window::getFrameSize().y);
    auto sizeCallback([&](const Message & msg_) {
        const WindowFrameSizeMessage & msg(static_cast<const WindowFrameSizeMessage &>(msg_));
        s_wasResize = true;
    });
    Scene::addReceiver<WindowFrameSizeMessage>(nullptr, sizeCallback);

    /* Init light */
    s_lightObject = &Scene::createGameObject();
    s_lightCamera = &Scene::addComponent<CameraComponent>(*s_lightObject, 45.f, -150.0f, 150.0f);
    glm::vec3 w = glm::normalize(glm::vec3(-0.2f, 0.75f, 0.5f));
    glm::vec3 u = glm::normalize(glm::cross(w, glm::vec3(0.f, 1.f, 0.f)));
    glm::vec3 v = glm::normalize(glm::cross(u, w));
    s_lightSpatial = &Scene::addComponent<SpatialComponent>(*s_lightObject, w * lightDist, glm::vec3(2.f), glm::mat3(u, v, w));
    s_lightCamera = &Scene::addComponent<CameraComponent>(*s_lightObject, glm::vec2(-75.f, 100.f), glm::vec2(-55.f, 100.f), 0.01f, 105.f);
    //Scene::addComponent<DiffuseRenderComponent>(*s_lightObject, *Loader::getMesh("cube.obj"), ModelTexture(1.f, glm::vec3(1.f), glm::vec3(0.9f)), true, glm::vec2(1, 1));

    /* Init shaders */
    if (!(    s_diffuseShader = UniquePtr<    DiffuseShader>::make(    "diffuse_vert.glsl",     "diffuse_frag.glsl")) ||
        !(    s_bounderShader = UniquePtr<    BounderShader>::make(    "bounder_vert.glsl",     "bounder_frag.glsl")) ||
        !(     s_octreeShader = UniquePtr<     OctreeShader>::make(    "bounder_vert.glsl",     "bounder_frag.glsl")) ||
        !(        s_rayShader = UniquePtr<        RayShader>::make(        "ray_vert.glsl",         "ray_frag.glsl")) ||
        !(s_postProcessShader = UniquePtr<PostProcessShader>::make("postprocess_vert.glsl", "postprocess_frag.glsl")) ||
        !(     s_shadowShader = UniquePtr<ShadowDepthShader>::make(     "shadow_vert.glsl",      "shadow_frag.glsl")) ||
        !(       s_blurShader = UniquePtr<       BlurShader>::make(       "pass_vert.glsl",        "blur_frag.glsl"))
    ) {
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }
    s_diffuseShader->init();
    s_bounderShader->init();
    s_octreeShader->init();
    s_rayShader->init();
    s_postProcessShader->init();
    s_shadowShader->init();
    s_blurShader->init();

    /* Init FBO */
    initFBO();
}

void RenderSystem::update(float dt) {
    /* Handle window resize */
    if (s_wasResize) {
        doResize();
        s_wasResize = false;
    }

    /* Update render components */
    for (DiffuseRenderComponent * comp : s_diffuseComponents) {
        comp->update(dt);
    }

    /* Update light */
    s_lightSpatial->setRelativePosition(getLightDir() * -lightDist + s_playerCamera->gameObject().getComponentByType<SpatialComponent>()->position());

    if (!s_playerCamera) {
        return;
    }

    /* Render shadow map */
    s_shadowShader->render(s_lightCamera);

    /* Set up post process FBO */
    if (s_postProcessShader->isEnabled()) {
        glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);
    }

    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::ivec2 size = Window::getFrameSize();
    glViewport(0, 0, size.x, size.y);

    /* Render! */
    s_diffuseShader->render(s_playerCamera);
    s_bounderShader->render(s_playerCamera);
    s_octreeShader->render(s_playerCamera);
    s_rayShader->render(s_playerCamera);

    /* Rebind screen FBO */
    if (s_postProcessShader->isEnabled()) {
        doBloom();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        s_postProcessShader->render(s_playerCamera);
    }
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

void RenderSystem::initFBO() {
    // Initialize framebuffer to draw into
    glGenFramebuffers(1, &s_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);

    glm::ivec2 size = Window::getFrameSize();

    //Create 2 color buffers (One for normal view, the other for frags with a high luminosity )
    glGenTextures(2, s_fboColorTexs);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, s_fboColorTexs[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB16F, size.x, size.y, 0, GL_RGB, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, s_fboColorTexs[i], 0
        );
    }


    // Create Renderbuffer Object to hold depth and stencil buffers
    GLuint fboDepthRB(0);
    glGenRenderbuffers(1, &fboDepthRB);
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepthRB);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepthRB);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    // ping-pong-framebuffer for blurring
    //Used for performing the vertical and horizontal blur
    glGenFramebuffers(2, s_pingpongFBO);
    glGenTextures(2, s_pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, s_pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, s_pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, size.x, size.y, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderSystem::doResize() {
    glViewport(0, 0, Window::getFrameSize().x, Window::getFrameSize().y);
    initFBO();
}

/* Frustum culling */
void RenderSystem::getFrustumComps(const CameraComponent *camera, Vector<DiffuseRenderComponent *> &comps) {
    for (auto comp : s_diffuseComponents) {
        if (camera->sphereInFrustum(comp->enclosingSphere())) {
            comps.push_back(comp);
        }
    }
}

void RenderSystem::doBloom() {
    //Blur bright frags
    bool horizontal = true, first_iteration = true;
    
    //How much we want to blur the bloom image
    unsigned int amount = 10;

    glActiveTexture(GL_TEXTURE0);
    
    //This is where the actual blurring occurs
    s_blurShader->bind();
    for (unsigned int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, s_pingpongFBO[horizontal]);
        s_blurShader->s_horizontal = horizontal;
        glBindTexture(GL_TEXTURE_2D, first_iteration ? s_fboColorTexs[1] : s_pingpongColorbuffers[!horizontal]); // bind texture of other framebuffer (or scene if first iteration)
        s_blurShader->render(nullptr);
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    s_blurShader->unbind();
}
