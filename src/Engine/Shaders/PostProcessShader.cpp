#include "PostProcessShader.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "IO/Window.hpp"
#include "System/RenderSystem.hpp"

PostProcessShader::PostProcessShader(const String & vertName, const String & fragName) :
    Shader(vertName, fragName)
{}

bool PostProcessShader::init() {
    if (!Shader::init()) {
        return false;
    }

    addAttribute("v_vPos");
    
    addUniform("v_scale");
    addUniform("v_translate");
    addUniform("v_depth");

    addUniform("f_texCol");

    tex_pizza = Loader::getTexture("chrome_tex.png");

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

void PostProcessShader::render(const CameraComponent * camera) {
    bind();

    glm::ivec2 size = Window::getFrameSize();

    glBindVertexArray(s_vaoHandle);

    loadVec2(getUniform("v_scale"), glm::vec2(1.f, 1.f));
    loadVec2(getUniform("v_translate"), glm::vec2(0.f, 0.f));
    loadFloat(getUniform("v_depth"), -.4f);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, RenderSystem::getFBOTexture());
    glUniform1i(getUniform("f_texCol"), 0);

    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void *) 0);

    // Assumes that width > height
    float xScale = (float) size.y / (float) size.x;

    loadVec2(getUniform("v_scale"), glm::vec2(.125f * xScale, .125f));
    loadVec2(getUniform("v_translate"), glm::vec2(.825f, .825f));
    loadFloat(getUniform("v_depth"), -.5f);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_pizza->textureId);
    glUniform1i(getUniform("f_texCol"), 0);

    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void *) 0);

    glBindVertexArray(0);
    
    // Unbind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    unbind();
}