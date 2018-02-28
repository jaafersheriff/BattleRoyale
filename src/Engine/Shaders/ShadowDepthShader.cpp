#include "ShadowDepthShader.hpp"

#include <glm/gtc/matrix_transform.hpp>

ShadowDepthShader::ShadowDepthShader(const String & vertName, const String & fragName, int width, int height) :
    Shader(vertName, fragName),
    mapWidth(width),
    mapHeight(height)
{}

bool ShadowDepthShader::init() {
    if (!Shader::init()) {
        return false;
    }

    addAttribute("vertPos");
    addAttribute("vertNor");
    addAttribute("vertTex"); // Unnecessary but avoids shader error messages

    addUniform("M");
    addUniform("LP");
    addUniform("LV");

    initFBO();

    return true;
}

void ShadowDepthShader::initFBO() {
    // generate the FBO for the shadow depth
    glGenFramebuffers(1, &fboHandle);

    // generate the texture
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mapWidth, mapHeight,
    	0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // bind with framebuffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fboTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowDepthShader::prepareRender(glm::vec3 & lightDir) {
    glViewport(0, 0, mapWidth, mapHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    bind();

    /* Calculate L */
    glm::mat4 LP = glm::ortho(-10.f, 10.f,  /* left, right */
                              -10.f, 10.f,  /* bottom, top */
                              0.1f, 250.f); /* near, far */
    loadMat4(getUniform("LP"), LP);
    glm::mat4 LV = glm::lookAt(lightDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    loadMat4(getUniform("LV"), LV);

    this->L = LP * LV;
}

void ShadowDepthShader::finishRender() {
    unbind();
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
