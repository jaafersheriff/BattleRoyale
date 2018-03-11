#include "SquareShader.hpp"
#include "IO/Window.hpp"

#include <glm/gtc/matrix_transform.hpp>

SquareShader::SquareShader(const String & vertName, const String & fragName) :
    Shader(vertName, fragName)
{}

bool SquareShader::init() {
    if (!Shader::init()) {
        return false;
    }

    addAttribute("v_screenPos");
    addUniform("f_texCol");

    initFBO();

    // Initialize frameSquarePos
    frameSquarePos = {
        1.f, 1.f,
        1.f, -1.f,
        -1.f, 1.f,
        -1.f, -1.f
    };

    // Give frameSquarePos to the GPU
    glGenBuffers(1, &frameSquarePosHandle);
    glBindBuffer(GL_ARRAY_BUFFER, frameSquarePosHandle);
    glBufferData(GL_ARRAY_BUFFER, frameSquarePos.size() * sizeof(float), 
        &frameSquarePos[0], GL_STATIC_DRAW);

    // Initialize frameSquareElem;
    frameSquareElem = {
        3, 0, 2,
        3, 1, 0
    };

    // Give frameSquareElem to the GPU
    glGenBuffers(1, &frameSquareElemHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frameSquareElemHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, frameSquareElem.size() * sizeof(unsigned),
        &frameSquareElem[0], GL_STATIC_DRAW);

    // Bind each of GL_ARRAY_BUFFER and GL_ELEMENT_ARRAY_BUFFER to default
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

void SquareShader::initFBO() {
    // Initialize texture to draw into
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

    glm::ivec2 size = Window::getFrameSize();

    // Set the first texture unit as active
    glActiveTexture(GL_TEXTURE0);

    // Attach color to the framebuffer
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0
    );

    // Attach depth to the framebuffer
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    // TODO: Pack it tighter ???
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void SquareShader::render(const CameraComponent * camera, const Vector<Component *> & components) {
    int pos;

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glUniform1i(getUniform("f_texCol"), 0);

    // Bind "vertex buffer"
    pos = getAttribute("v_screenPos");
    glEnableVertexAttribArray(pos);
    glBindBuffer(GL_ARRAY_BUFFER, frameSquarePosHandle);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 
        sizeof(GL_FLOAT) * 2, (const void *) 0);

    // Bind "element buffer"
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frameSquareElemHandle);

    // Draw
    glDrawElements(GL_TRIANGLES, (int) frameSquareElem.size(),
        GL_UNSIGNED_INT, (const void *) 0);

    // Disable
    glDisableVertexAttribArray(frameSquarePosHandle);

    // Unbind all
    
    // Unbind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}