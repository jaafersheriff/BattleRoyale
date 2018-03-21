#include "HealthShader.hpp"

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLSL.hpp"

#include "System/RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/EnemyComponents/EnemyComponent.hpp"
#include "Component/StatComponents/StatComponents.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

HealthShader::HealthShader(const String & vertName, const String & fragName) :
    Shader(vertName, fragName) 
{}

bool HealthShader::init() {
    if (!Shader::init()) {
        return false;
    }

    m_size = glm::vec2(0.7f, 0.1f);

    addAttribute("vertPos");
    
    addUniform("P");
    addUniform("V");

    addUniform("center");
    addUniform("size");

    addUniform("minVal");
    addUniform("curVal");
    addUniform("maxVal");

    if (!initQuad()) {
        std::cerr << "Error initalizing quad mesh" << std::endl;
    }

    return true;
}

void HealthShader::render(const CameraComponent * camera) {
    if (!camera || !m_isEnabled) {
        return;
    }
    
    bind();

    /* Bind quad */
    glBindVertexArray(m_quadVAO);
    int pos = getAttribute("vertPos");
    glEnableVertexAttribArray(pos);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

    loadMat4(getUniform("P"), camera->getProj());
    loadMat4(getUniform("V"), camera->getView());

    loadVec2(getUniform("size"), m_size);

    /* Iterate through all enemies */
    auto enemies = Scene::getComponents<EnemyComponent>();
    for (auto enemy : enemies) {
        auto health = enemy->gameObject().getComponentByType<HealthComponent>();
        auto spatials = enemy->gameObject().getComponentsByType<SpatialComponent>();
        if (!health || !spatials.size()) {
            return;
        }

        /* Store health params */
        loadFloat(getUniform("minVal"), health->minValue());
        loadFloat(getUniform("curVal"), health->value());
        loadFloat(getUniform("maxVal"), health->maxValue());

        /* Find position based on hierarchy */
        glm::vec3 position = spatials[0]->position();
        for (auto spatial : spatials) {
            position += glm::vec3(0.f, spatial->scale().y, 0.f);
        }
        loadVec3(getUniform("center"), position + glm::vec3(0.f, 0.2f, 0.f));

        /* Draw */
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glDisableVertexAttribArray(getAttribute("vertPos"));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    unbind();
}

bool HealthShader::initQuad() {
    int nVerts(4);
    glm::vec3 verts[4]{
        { -0.5f, -0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f },
        { -0.5f,  0.5f, 0.0f },
        {  0.5f,  0.5f, 0.0f },
    };

    glGenVertexArrays(1, &m_quadVAO);
    glBindVertexArray(m_quadVAO);

    glGenBuffers(1, &m_quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, nVerts * sizeof(glm::vec3) , verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Error check */
    return (glGetError() == GL_NO_ERROR);
}