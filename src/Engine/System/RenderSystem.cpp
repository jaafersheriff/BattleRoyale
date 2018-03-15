#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"
#include "Scene/Scene.hpp"

const Vector<DiffuseRenderComponent *> & RenderSystem::s_diffuseComponents(Scene::getComponents<DiffuseRenderComponent>());
/* FBO */
GLuint RenderSystem::s_fbo = 0;
GLuint RenderSystem::s_fboColorTex = 0;
bool RenderSystem::s_wasResize = false;
/* Camera and light */
const CameraComponent * RenderSystem::s_playerCamera = nullptr;
GameObject * RenderSystem::s_lightObject = nullptr;
CameraComponent * RenderSystem::s_lightCamera = nullptr;
SpatialComponent * RenderSystem::s_lightSpatial = nullptr;
float RenderSystem::lightDist = 15.f; // TODO : fix
/* Shaders */
ShadowDepthShader * RenderSystem::s_shadowShader = nullptr;
DiffuseShader * RenderSystem::s_diffuseShader = nullptr;
BounderShader * RenderSystem::s_bounderShader = nullptr;
RayShader * RenderSystem::s_rayShader = nullptr;
OctreeShader * RenderSystem::s_octreeShader = nullptr;
PostProcessShader * RenderSystem::s_postProcessShader = nullptr;
 

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
    s_lightCamera = &Scene::addComponent<CameraComponent>(*s_lightObject, 45.f, 0.01f, 300.f);
    glm::vec3 w = glm::normalize(glm::vec3(-0.2f, 0.75f, 0.5f));
    glm::vec3 u = glm::normalize(glm::cross(w, glm::vec3(0.f, 1.f, 0.f)));
    glm::vec3 v = glm::normalize(glm::cross(u, w));
    s_lightSpatial = &Scene::addComponent<SpatialComponent>(*s_lightObject, w * lightDist, glm::vec3(2.f), glm::mat3(u, v, w));
    s_lightCamera = &Scene::addComponent<CameraComponent>(*s_lightObject, glm::vec2(-75.f, 100.f), glm::vec2(-55.f, 100.f), 0.01f, 105.f);
    //Scene::addComponent<DiffuseRenderComponent>(*s_lightObject, *Loader::getMesh("cube.obj"), ModelTexture(1.f, glm::vec3(1.f), glm::vec3(0.9f)), true, glm::vec2(1, 1));

    /* Init post process shader */
    if (!createPostProcessShader("postprocess_vert.glsl", "postprocess_frag.glsl")) {
        std::exit(EXIT_FAILURE);
    }

    /* Init shadow shader */
    if (!createShadowShader("shadow_vert.glsl", "shadow_frag.glsl")) {
        std::exit(EXIT_FAILURE);
    }

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
    s_lightSpatial->setPosition(getLightDir() * -lightDist + s_playerCamera->gameObject().getComponentByType<SpatialComponent>()->position());

    /* Render shadow map */
    Vector<DiffuseRenderComponent *> shadowCasters = getFrustumComps(s_lightCamera);
    s_shadowShader->render(s_lightCamera, shadowCasters);

    /* Set up post process FBO */
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);

    if (!s_playerCamera) {
        return;
    }
    
    /* Get components in frustum */
    Vector<DiffuseRenderComponent *> compsToRender = getFrustumComps(s_playerCamera);

    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::ivec2 size = Window::getFrameSize();
    glViewport(0, 0, size.x, size.y);

    /* Render! */
    s_diffuseShader->render(s_playerCamera, compsToRender);
    s_rayShader->render(s_playerCamera, compsToRender);
    s_bounderShader->render(s_playerCamera, compsToRender);

    /* Rebind screen FBO */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* Post processing */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    s_postProcessShader->render(nullptr, *((Vector<DiffuseRenderComponent *> *) nullptr));

#ifdef DEBUG_MODE
    if (Window::isImGuiEnabled()) {
        ImGui::Render();
    }
#endif
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderSystem::doResize() {
    glViewport(0, 0, Window::getFrameSize().x, Window::getFrameSize().y);
    initFBO();
}

/* Frustum culling */
// TODO : return a reference?
Vector<DiffuseRenderComponent *> RenderSystem::getFrustumComps(const CameraComponent *camera) {
    Vector<DiffuseRenderComponent *> renderComps;
    for (auto comp : s_diffuseComponents) {
        if (camera->sphereInFrustum(comp->enclosingSphere())) {
            renderComps.push_back(comp);
        }
    }
    return renderComps;
}











bool RenderSystem::createDiffuseShader(String vert, String frag) {
s_diffuseShader = new DiffuseShader(vert, frag);
return initShader(s_diffuseShader);
}

bool RenderSystem::createBounderShader(String vert, String frag) {
s_bounderShader = new BounderShader(vert, frag);
return initShader(s_bounderShader);
}

bool RenderSystem::createRayShader(String vert, String frag) {
s_rayShader = new RayShader(vert, frag);
return initShader(s_rayShader);
}

bool RenderSystem::createOctreeShader(String vert, String frag) {
s_octreeShader = new OctreeShader(vert, frag);
return initShader(s_octreeShader);
}

bool RenderSystem::createPostProcessShader(String vert, String frag) {
s_postProcessShader = new PostProcessShader(vert, frag);
return initShader(s_postProcessShader);
}

bool RenderSystem::createShadowShader(String vert, String frag) {
s_shadowShader = new ShadowDepthShader(vert, frag);
return initShader(s_shadowShader);
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