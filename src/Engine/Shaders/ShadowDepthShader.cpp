#include "ShadowDepthShader.hpp"

#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/RenderComponents/DiffuseRenderComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

#include "System/RenderSystem.hpp"

#include <glm/gtc/matrix_transform.hpp>

#define DEFAULT_SIZE 1024

ShadowDepthShader::ShadowDepthShader(const String & vertName, const String & fragName) :
    Shader(vertName, fragName) {
    s_shadowMap = new Texture;
    s_shadowMap->width = s_shadowMap->height = DEFAULT_SIZE;
}

bool ShadowDepthShader::init() {
    if (!Shader::init()) {
        return false;
    }

    addAttribute("vertPos");

    addUniform("L");
    addUniform("M");

    initFBO();

    return true;
}

void ShadowDepthShader::initFBO() {
    // generate the FBO for the shadow depth
    glGenFramebuffers(1, &s_fboHandle);

    // generate the texture
    glGenTextures(1, &s_shadowMap->textureId);
    glBindTexture(GL_TEXTURE_2D, s_shadowMap->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, s_shadowMap->width, s_shadowMap->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // bind with framebuffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, s_fboHandle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, s_shadowMap->textureId, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowDepthShader::setMapSize(int size) {
    s_shadowMap->width = s_shadowMap->height = size;
    glBindTexture(GL_TEXTURE_2D, s_shadowMap->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowDepthShader::render(const CameraComponent * camera) {
    /* Reset shadow map */
    glViewport(0, 0, s_shadowMap->width, s_shadowMap->height);
    glBindFramebuffer(GL_FRAMEBUFFER, s_fboHandle);
    glClear(GL_DEPTH_BUFFER_BIT);
 
    if (!camera || !m_isEnabled) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    bind();
    glCullFace(GL_FRONT);

    /* Calculate L */
    this->L = camera->getProj() * camera->getView();
    loadMat4(getUniform("L"), L);

    Vector<DiffuseRenderComponent *> components;
    RenderSystem::getFrustumComps(camera, components);
    for (auto drc : components) {
    
        loadMat4(getUniform("M"), drc->gameObject().getSpatial()->modelMatrix());

        /* Bind mesh */
        const Mesh & mesh(drc->mesh());
        glBindVertexArray(mesh.vaoId);
            
        /* Bind vertex buffer VBO */
        int pos = getAttribute("vertPos");
        glEnableVertexAttribArray(pos);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vertBufId);
        glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        /* Bind indices buffer VBO */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eleBufId);

        /* DRAW */
        glDrawElements(GL_TRIANGLES, (int)mesh.eleBufSize, GL_UNSIGNED_INT, nullptr);

        /* Unload mesh */
        glDisableVertexAttribArray(getAttribute("vertPos"));
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glCullFace(GL_BACK);
    unbind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
