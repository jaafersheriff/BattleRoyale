#include "BlurShader.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "IO/Window.hpp"
#include "System/RenderSystem.hpp"

BlurShader::BlurShader(const String & vertName, const String & fragName) :
    Shader(vertName, fragName)
{}

bool BlurShader::init() {
    if (!Shader::init()) {
        return false;
    }

    addAttribute("v_screenPos");
    addUniform("image");
    addUniform("horizontal");

    // Initialize frameSquarePos
    glm::vec2 frameSquarePos[4]{
        glm::vec2(1.f, 1.f),
        glm::vec2(1.f, -1.f),
        glm::vec2(-1.f, 1.f),
        glm::vec2(-1.f, -1.f)
    };

    glGenVertexArrays(1, &s_vaoHandle);
    glBindVertexArray(s_vaoHandle);

    // Give frameSquarePos to the GPU
    glGenBuffers(1, &s_vboHandle);
    glBindBuffer(GL_ARRAY_BUFFER, s_vboHandle);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), frameSquarePos, GL_STATIC_DRAW);

    // Initialize frameSquareElem;
    unsigned int frameSquareElem[6]{
        3, 0, 2,
        3, 1, 0
    };

    // Give frameSquareElem to the GPU
    glGenBuffers(1, &s_iboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_iboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), frameSquareElem, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Bind each of GL_ARRAY_BUFFER and GL_ELEMENT_ARRAY_BUFFER to default
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    

    return true;
}

void BlurShader::render(const CameraComponent * camera, const Vector<Component *> & components){
    // Bind texture
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, RenderSystem::colorBuffers[1]);
    glUniform1i(getUniform("image"), 0);
    glUniform1i(getUniform("horizontal"), s_Horizontal);

    glBindVertexArray(s_vaoHandle);

    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void *)0);

    //glBindVertexArray(0);

    // Unbind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}