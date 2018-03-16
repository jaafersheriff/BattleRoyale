#include "DiffuseShader.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Component/RenderComponents/DiffuseRenderComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"

#include "System/CollisionSystem.hpp"
#include "System/RenderSystem.hpp"

DiffuseShader::DiffuseShader(const String & vertFile, const String & fragFile) :
    Shader(vertFile, fragFile) {
    cellIntensities.resize(1, 1.f);
    cellDiffuseScales.resize(1, 1.f);
    cellSpecularScales.resize(1, 1.f);
}

bool DiffuseShader::init() {
    if (!Shader::init()) {
        return false;
    }
    /* Add attributes */
    addAttribute("vertPos");
    addAttribute("vertNor");
    addAttribute("vertTex");

    /* Add uniforms */
    addUniform("P");
    addUniform("V");
    addUniform("M");
    addUniform("N");
    
    addUniform("L");
    addUniform("shadowMap");

    addUniform("tiling");

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
    glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 16*4);
    GLSL::checkError();

    glGenTextures(1, &cellDiffuseScalesTexture);
    glActiveTexture(GL_TEXTURE0 + cellDiffuseScalesTexture);
    glBindTexture(GL_TEXTURE_1D, cellDiffuseScalesTexture);
    glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 16*4);
    GLSL::checkError();

    glGenTextures(1, &cellSpecularScalesTexture);
    glActiveTexture(GL_TEXTURE0 + cellSpecularScalesTexture);
    glBindTexture(GL_TEXTURE_1D, cellSpecularScalesTexture);
    glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 16*4);
    GLSL::checkError();

    glBindTexture(GL_TEXTURE_1D, 0);
    GLSL::checkError();

    return true;
}

void DiffuseShader::render(const CameraComponent * camera) {
    if (!camera || !m_isEnabled) {
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

    /* Shadows */
    loadMat4(getUniform("L"), RenderSystem::getL());
    GLuint shadowMap = RenderSystem::getShadowMap();
    glActiveTexture(GL_TEXTURE0 + shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    loadInt(getUniform("shadowMap"), shadowMap);

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

    /* Get render targets */
    Vector<DiffuseRenderComponent *> components;
    RenderSystem::getFrustumComps(camera, components);

    /* Iterate through render targets */
    for (auto drc : components) {

        /* Toon shading */
        if (showToon && drc->isToon()) {
            loadBool(getUniform("isToon"), true);
        }
        else {
            loadBool(getUniform("isToon"), false);
        }

        /* Tiling amount */
        loadVec2(getUniform("tiling"), drc->tiling());

        /* Model matrix */
        loadMat4(getUniform("M"), drc->gameObject().getSpatial()->modelMatrix());
        /* Normal matrix */
        loadMat3(getUniform("N"), drc->gameObject().getSpatial()->normalMatrix());

        /* Bind materials */
        const ModelTexture modelTexture(drc->modelTexture());
        loadFloat(getUniform("matAmbient"), modelTexture.material.ambient);
        loadVec3(getUniform("matDiffuse"), modelTexture.material.diffuse);
        loadVec3(getUniform("matSpecular"), modelTexture.material.specular);
        loadFloat(getUniform("shine"), modelTexture.material.shineDamper);
   
        /* Load texture */
        if(modelTexture.texture && modelTexture.texture->textureId != 0) {
            loadBool(getUniform("usesTexture"), true);
            loadInt(getUniform("textureImage"), modelTexture.texture->textureId);
            glActiveTexture(GL_TEXTURE0 + modelTexture.texture->textureId);
            glBindTexture(GL_TEXTURE_2D, modelTexture.texture->textureId);
        }
        else {
            loadBool(getUniform("usesTexture"), false);
        }

        /* Bind mesh */
        const Mesh & mesh(drc->mesh());
        glBindVertexArray(mesh.vaoId);
            
        /* Bind vertex buffer VBO */
        int pos = getAttribute("vertPos");
        glEnableVertexAttribArray(pos);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vertBufId);
        glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        /* Bind normal buffer VBO */
        pos = getAttribute("vertNor");
        if (pos != -1 && mesh.norBufId != 0) {
            glEnableVertexAttribArray(pos);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.norBufId);
            glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        /* Bind texture coordinate buffer VBO */
        pos = getAttribute("vertTex");
        if (pos != -1 && mesh.texBufId != 0) {
            glEnableVertexAttribArray(pos);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.texBufId);
            glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        /* Bind indices buffer VBO */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eleBufId);

        /* DRAW */
        glDrawElements(GL_TRIANGLES, (int)mesh.eleBufSize, GL_UNSIGNED_INT, nullptr);

        /* Unload mesh */
        glDisableVertexAttribArray(getAttribute("vertPos"));
        pos = getAttribute("vertNor");
        if (pos != -1) {
            glDisableVertexAttribArray(pos);
        }
        pos = getAttribute("vertTex");
        if (pos != -1) {
            glDisableVertexAttribArray(pos);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_1D, 0);

        /* Unload texture */
        if (modelTexture.texture) {
            glActiveTexture(GL_TEXTURE0 + modelTexture.texture->textureId);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
 
    }

    if (showWireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }    

    unbind();
}

void DiffuseShader::setCells(unsigned int in) {
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

void DiffuseShader::setCellIntensity(unsigned int i, float f) {
    cellIntensities[i] = (i == 0) ? f : glm::min(cellIntensities[i - 1], f);
}

void DiffuseShader::setCellDiffuseScale(unsigned int i, float f) {
    cellDiffuseScales[i] = (i == 0) ? f : glm::min(cellDiffuseScales[i - 1], f);
}
void DiffuseShader::setCellSpecularScale(unsigned int i, float f) {
    cellSpecularScales[i] = (i == 0) ? f : glm::min(cellSpecularScales[i - 1], f);
}