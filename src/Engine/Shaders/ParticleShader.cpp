#include "ParticleShader.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "System/CollisionSystem.hpp"
#include "System/RenderSystem.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/ParticleComponents/ParticleComponent.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"

ParticleShader::ParticleShader(const String & vertFile, const String & fragFile) :
    Shader(vertFile, fragFile),
    particlesVAO(0),
    instanceVBO(0)
{
    cellIntensities.resize(1, 1.f);
    cellDiffuseScales.resize(1, 1.f);
    cellSpecularScales.resize(1, 1.f);
}

bool ParticleShader::init() {
    if (!Shader::init()) {
        return false;
    }
    /* Add attributes */
    addAttribute("vertPos");
    addAttribute("vertNor");
    addAttribute("vertTex");
    addAttribute("particlePosition");
    addAttribute("particleMatrixID");
    addAttribute("particleVelocity");
    addAttribute("particleLife");

    // Init particles buffers
    glGenVertexArrays(1, &particlesVAO);
    glBindVertexArray(particlesVAO);

    // Instancing OpenGL Stuff
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
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


    /* Add uniforms */
    addUniform("P");
    addUniform("V");
    addUniform("variationMs");
    addUniform("variationNs");
    addUniform("particleScale");
    addUniform("particleVariation");
    addUniform("particleFade");
    addUniform("particleMaxLife");

    addUniform("lightDir");
    addUniform("camPos");

    addUniform("matAmbient");
    addUniform("matDiffuse");
    addUniform("matSpecular");
    addUniform("shine");
    addUniform("textureImage");
    addUniform("usesTexture");

    addUniform("isToon");
    addUniform("silAngle");
    addUniform("numCells");
    addUniform("cellIntensities");
    addUniform("cellDiffuseScales");
    addUniform("cellSpecularScales");

    /* Generate 1D Textures with initial size of 16 floats */
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &cellIntensitiesTexture);
    glBindTexture(GL_TEXTURE_1D, cellIntensitiesTexture);
    glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 16 * 4);
    GLSL::checkError();

    glGenTextures(1, &cellDiffuseScalesTexture);
    glActiveTexture(GL_TEXTURE0 + cellDiffuseScalesTexture);
    glBindTexture(GL_TEXTURE_1D, cellDiffuseScalesTexture);
    glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 16 * 4);
    GLSL::checkError();

    glGenTextures(1, &cellSpecularScalesTexture);
    glActiveTexture(GL_TEXTURE0 + cellSpecularScalesTexture);
    glBindTexture(GL_TEXTURE_1D, cellSpecularScalesTexture);
    glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 16 * 4);
    GLSL::checkError();

    return true;
}

void ParticleShader::render(const CameraComponent * camera) {
    if (!camera) {
        return;
    }

    bind();

    if (showWireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    /* Bind uniforms */
    loadMat4(getUniform("P"), camera->getProj());
    loadMat4(getUniform("V"), camera->getView());
    loadVec3(getUniform("lightDir"), RenderSystem::getLightDir());
    loadVec3(getUniform("camPos"), camera->gameObject().getSpatial()->position());

    /* Toon shading */
    loadFloat(getUniform("silAngle"), silAngle);
    loadFloat(getUniform("numCells"), (float)numCells);
    loadInt(getUniform("cellIntensities"), cellIntensitiesTexture);
    loadInt(getUniform("cellDiffuseScales"), cellDiffuseScalesTexture);
    loadInt(getUniform("cellSpecularScales"), cellSpecularScalesTexture);

    // TODO : move cell intensities and scales to material and initialize it during json pass
    // TODO : only upload this data once when the material is loaded in
    glActiveTexture(GL_TEXTURE0 + cellIntensitiesTexture);
    glBindTexture(GL_TEXTURE_1D, cellIntensitiesTexture);
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, int(cellIntensities.size()), GL_RED, GL_FLOAT, cellIntensities.data());
    glActiveTexture(GL_TEXTURE0 + cellDiffuseScalesTexture);
    glBindTexture(GL_TEXTURE_1D, cellDiffuseScalesTexture);
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, int(cellDiffuseScales.size()), GL_RED, GL_FLOAT, cellDiffuseScales.data());
    glActiveTexture(GL_TEXTURE0 + cellSpecularScalesTexture);
    glBindTexture(GL_TEXTURE_1D, cellSpecularScalesTexture);
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, int(cellSpecularScales.size()), GL_RED, GL_FLOAT, cellSpecularScales.data());
    
    // Load variation matrices
    loadMultiMat4(getUniform("variationMs"), ParticleSystem::m_variationMs.data(), ParticleSystem::k_maxVariations);        
    loadMultiMat3(getUniform("variationNs"), ParticleSystem::m_variationNs.data(), ParticleSystem::k_maxVariations);
    
    glBindVertexArray(particlesVAO);

    for (ParticleComponent * pc : ParticleSystem::s_particleComponents) {
        if (pc->finished()) {
            continue;
        }

        /* Toon shading */
        if (showToon) {
            loadBool(getUniform("isToon"), true);
        }
        else {
            loadBool(getUniform("isToon"), false);
        }

        // Load particle uniforms
        loadFloat(getUniform("particleScale"), pc->m_scale);
        loadBool(getUniform("particleVariation"), pc->m_variation);
        loadBool(getUniform("particleFade"), pc->m_fade);
        loadFloat(getUniform("particleMaxLife"), pc->m_duration);

        /* Bind materials */
        loadFloat(getUniform("matAmbient"), pc->m_modelTexture.material.ambient);
        loadVec3(getUniform("matDiffuse"), pc->m_modelTexture.material.diffuse);
        loadVec3(getUniform("matSpecular"), pc->m_modelTexture.material.specular);
        loadFloat(getUniform("shine"), pc->m_modelTexture.material.shineDamper);
   
        /* Load texture */
        if(pc->m_modelTexture.texture && pc->m_modelTexture.texture->textureId != 0) {
            loadBool(getUniform("usesTexture"), true);
            loadInt(getUniform("textureImage"), pc->m_modelTexture.texture->textureId);
            glActiveTexture(GL_TEXTURE0 + pc->m_modelTexture.texture->textureId);
            glBindTexture(GL_TEXTURE_2D, pc->m_modelTexture.texture->textureId);
        }
        else {
            loadBool(getUniform("usesTexture"), false);
        }

        // Buffer instance data
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, pc->m_particles.size() * sizeof(Particle), pc->m_particles.data());

        /* Bind vertex buffer VBO */
        int pos = getAttribute("vertPos");
        glBindBuffer(GL_ARRAY_BUFFER, pc->m_mesh.vertBufId);
        glEnableVertexAttribArray(pos);
        glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        /* Bind normal buffer VBO */
        pos = getAttribute("vertNor");
        if (pos != -1 && pc->m_mesh.norBufId != 0) {
            glBindBuffer(GL_ARRAY_BUFFER, pc->m_mesh.norBufId);
            glEnableVertexAttribArray(pos);
            glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        /* Bind texture coordinate buffer VBO */
        pos = getAttribute("vertTex");
        if (pos != -1 && pc->m_mesh.texBufId != 0) {
            glBindBuffer(GL_ARRAY_BUFFER, pc->m_mesh.texBufId);
            glEnableVertexAttribArray(pos);
            glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        /* Bind indices buffer VBO */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pc->m_mesh.eleBufId);

        /* DRAW */
        glDrawElementsInstanced(GL_TRIANGLES, (int)pc->m_mesh.eleBufSize, GL_UNSIGNED_INT, nullptr, int(pc->m_particles.size()));

        /* Unload texture */
        if (pc->m_modelTexture.texture) {
            glActiveTexture(GL_TEXTURE0 + pc->m_modelTexture.texture->textureId);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
 
    }

    glBindTexture(GL_TEXTURE_1D, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (showWireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } 

    unbind();   
}


void ParticleShader::setCells(unsigned int in) {
    numCells = glm::min(in, (unsigned int)16);
    cellIntensities.resize(numCells, 0.f);
    cellDiffuseScales.resize(numCells, 0.f);
    cellSpecularScales.resize(numCells, 0.f);
    for (int i = 0; i < int(numCells); i++) {
        float scale = 1.f - i / (float)numCells;
        cellIntensities[i] = (scale - 0.5f) * 2.0f;
        cellDiffuseScales[i] = scale;
        cellSpecularScales[i] = scale;
    }
}

void ParticleShader::setCellIntensity(unsigned int i, float f) {
    cellIntensities[i] = (i == 0) ? f : glm::min(cellIntensities[i - 1], f);
}

void ParticleShader::setCellDiffuseScale(unsigned int i, float f) {
    cellDiffuseScales[i] = (i == 0) ? f : glm::min(cellDiffuseScales[i - 1], f);
}
void ParticleShader::setCellSpecularScale(unsigned int i, float f) {
    cellSpecularScales[i] = (i == 0) ? f : glm::min(cellSpecularScales[i - 1], f);
}