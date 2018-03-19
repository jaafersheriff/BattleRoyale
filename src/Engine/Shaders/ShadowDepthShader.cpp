#include "ShadowDepthShader.hpp"

#define GLEW_STATIC
#include "GL/glew.h"
#include <glm/gtc/matrix_transform.hpp>
#include "GLSL.hpp"

#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/RenderComponents/DiffuseRenderComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "System/RenderSystem.hpp"
#include "Model/Mesh.hpp"
#include "System/ParticleSystem.hpp"
#include "Component/ParticleComponents/ParticleComponent.hpp"

#define DEFAULT_SIZE 8192

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

    //--------------------------------------------------------------------------
    // Particles

    /* Add attributes */
    addAttribute("particlePosition");
    addAttribute("particleMatrixID");
    addAttribute("particleVelocity");
    addAttribute("particleLife");

    /* Add uniforms */
    addUniform("particles");
    addUniform("variationMs");
    addUniform("particleScale");
    addUniform("particleVariation");
    addUniform("particleFade");
    addUniform("particleMaxLife");

    // Init particles buffers
    glGenVertexArrays(1, &s_particlesVAO);
    glBindVertexArray(s_particlesVAO);

    glGenBuffers(1, &s_particlesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, s_particlesVBO);
    glBufferData(GL_ARRAY_BUFFER, ParticleSystem::k_maxNParticles * sizeof(Particle), nullptr, GL_STREAM_DRAW);
    // Position
    int positionAI(getAttribute("particlePosition"));
    if (positionAI != -1) {
        glEnableVertexAttribArray(positionAI);
        glVertexAttribPointer(positionAI, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)0);
        glVertexAttribDivisor(positionAI, 1);
    }
    // Matrix ID
    int matrixIDAI(getAttribute("particleMatrixID"));
    if (matrixIDAI != -1) {
        glEnableVertexAttribArray(matrixIDAI);
        glVertexAttribIPointer(matrixIDAI, 1, GL_UNSIGNED_INT, sizeof(Particle), (void *)12);
        glVertexAttribDivisor(matrixIDAI, 1);
    }
    // Velocity
    int velocityAI(getAttribute("particleVelocity"));
    if (velocityAI != -1) {
        glEnableVertexAttribArray(velocityAI);
        glVertexAttribPointer(velocityAI, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)16);
        glVertexAttribDivisor(velocityAI, 1);
    }
    // Life
    int lifeAI(getAttribute("particleLife"));
    if (lifeAI != -1) {
        glEnableVertexAttribArray(lifeAI);
        glVertexAttribPointer(lifeAI, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)28);
        glVertexAttribDivisor(lifeAI, 1);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Load particle variation matrices
    bind();
    loadMultiMat4(getUniform("variationMs"), ParticleSystem::s_variationMs.data(), ParticleSystem::k_maxVariations);
    unbind();

    GLSL::checkError();

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
    
    // Not doing particles yet
    loadBool(getUniform("particles"), false);

    /* Calculate L */
    this->L = camera->getProj() * camera->getView();
    loadMat4(getUniform("L"), L);

    Vector<DiffuseRenderComponent *> components;
    RenderSystem::getFrustumComps(camera, components);
    for (auto drc : components) {
    
        loadMat4(getUniform("M"), drc->m_spatial->modelMatrix());

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

    //--------------------------------------------------------------------------
    // Particles
    
    loadBool(getUniform("particles"), true);
    
    glBindVertexArray(s_particlesVAO);

    for (ParticleComponent * pc : ParticleSystem::s_particleComponents) {
        if (pc->finished()) {
            continue;
        }

        // Load particle uniforms
        loadFloat(getUniform("particleScale"), pc->m_scale);
        loadBool(getUniform("particleVariation"), pc->m_variation);
        loadBool(getUniform("particleFade"), pc->m_fade);
        loadFloat(getUniform("particleMaxLife"), pc->m_duration);

        // Buffer instance data
        glBindBuffer(GL_ARRAY_BUFFER, s_particlesVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, pc->m_particles.size() * sizeof(Particle), pc->m_particles.data());

        /* Bind vertex buffer VBO */
        int pos = getAttribute("vertPos");
        glBindBuffer(GL_ARRAY_BUFFER, pc->m_mesh.vertBufId);
        glEnableVertexAttribArray(pos);
        glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        /* Bind indices buffer VBO */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pc->m_mesh.eleBufId);

        /* DRAW */
        glDrawElementsInstanced(GL_TRIANGLES, (int)pc->m_mesh.eleBufSize, GL_UNSIGNED_INT, nullptr, int(pc->m_particles.size())); 
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glCullFace(GL_BACK);
    unbind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
