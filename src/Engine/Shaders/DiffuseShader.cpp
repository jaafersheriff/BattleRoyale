#include "DiffuseShader.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Component/RenderComponents/DiffuseRenderComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "System/CollisionSystem.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"

DiffuseShader::DiffuseShader(const String & vertFile, const String & fragFile, const glm::vec3 & light) :
    Shader(vertFile, fragFile),
    lightDir(&light) {
    cellIntensities.resize(1, 1.f);
    cellScales.resize(1, 1.f);
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
    addUniform("cellScales");
    
    /* Generate 1D Textures with initial size of 16 floats */
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &cellIntensitiesTexture);
    glBindTexture(GL_TEXTURE_1D, cellIntensitiesTexture);
    glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 16*4);
    GLSL::checkError();

    glGenTextures(1, &cellScalesTexture);
    glActiveTexture(GL_TEXTURE0 + cellScalesTexture);
    glBindTexture(GL_TEXTURE_1D, cellScalesTexture);
    glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 16*4);
    GLSL::checkError();

    glBindTexture(GL_TEXTURE_1D, 0);
    assert(glGetError() == GL_NO_ERROR);

    return true;
}

void DiffuseShader::render(const CameraComponent * camera, const Vector<Component *> & components) {
    if (!camera) {
        return;
    }

    if (showWireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    /* Bind uniforms */
    loadMat4(getUniform("P"), camera->getProj());
    loadMat4(getUniform("V"), camera->getView());
    loadVec3(getUniform("lightDir"), *lightDir);
    loadVec3(getUniform("camPos"), camera->gameObject()->getSpatial()->position());

    /* Toon shading */
    loadFloat(getUniform("silAngle"), silAngle);
    loadFloat(getUniform("numCells"), (float)numCells);
    loadInt(getUniform("cellIntensities"), cellIntensitiesTexture);
    glActiveTexture(GL_TEXTURE0 + cellIntensitiesTexture);
    glBindTexture(GL_TEXTURE_1D, cellIntensitiesTexture);
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, cellIntensities.size(), GL_RED, GL_FLOAT, cellIntensities.data());
    loadInt(getUniform("cellScales"), cellScalesTexture);
    glActiveTexture(GL_TEXTURE0 + cellScalesTexture);
    glBindTexture(GL_TEXTURE_1D, cellScalesTexture);
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, cellScales.size(), GL_RED, GL_FLOAT, cellScales.data());

    for (Component * comp : components) {
        // TODO : component list should be passed in as diffuserendercomponent
        DiffuseRenderComponent *drc;
        if (!(drc = dynamic_cast<DiffuseRenderComponent *>(comp)) || drc->pid != this->pid) {
            continue;
        }

        /* Toon shading */
        if (showToon && drc->isToon) {
            loadBool(getUniform("isToon"), true);
        }
        else {
            loadBool(getUniform("isToon"), false);
        }

        /* Model matrix */
        loadMat4(getUniform("M"), drc->gameObject().getSpatial()->modelMatrix());
        /* Normal matrix */
        loadMat3(getUniform("N"), drc->gameObject().getSpatial()->normalMatrix());

        /* Bind materials */
        loadFloat(getUniform("matAmbient"), drc->modelTexture.material.ambient);
        loadVec3(getUniform("matDiffuse"), drc->modelTexture.material.diffuse);
        loadVec3(getUniform("matSpecular"), drc->modelTexture.material.specular);
        loadFloat(getUniform("shine"), drc->modelTexture.material.shineDamper);
   
        /* Load texture */
        if(drc->modelTexture.texture && drc->modelTexture.texture->textureId != 0) {
            loadBool(getUniform("usesTexture"), true);
            loadInt(getUniform("textureImage"), drc->modelTexture.texture->textureId);
            glActiveTexture(GL_TEXTURE0 + drc->modelTexture.texture->textureId);
            glBindTexture(GL_TEXTURE_2D, drc->modelTexture.texture->textureId);
        }
        else {
            loadBool(getUniform("usesTexture"), false);
        }

        /* Bind mesh */
        glBindVertexArray(drc->mesh->vaoId);
            
        /* Bind vertex buffer VBO */
        int pos = getAttribute("vertPos");
        glEnableVertexAttribArray(pos);
        glBindBuffer(GL_ARRAY_BUFFER, drc->mesh->vertBufId);
        glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        /* Bind normal buffer VBO */
        pos = getAttribute("vertNor");
        if (pos != -1 && drc->mesh->norBufId != 0) {
            glEnableVertexAttribArray(pos);
            glBindBuffer(GL_ARRAY_BUFFER, drc->mesh->norBufId);
            glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        /* Bind texture coordinate buffer VBO */
        pos = getAttribute("vertTex");
        if (pos != -1 && drc->mesh->texBufId != 0) {
            glEnableVertexAttribArray(pos);
            glBindBuffer(GL_ARRAY_BUFFER, drc->mesh->texBufId);
            glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        /* Bind indices buffer VBO */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drc->mesh->eleBufId);

        /* DRAW */
        glDrawElements(GL_TRIANGLES, (int)drc->mesh->eleBufSize, GL_UNSIGNED_INT, nullptr);

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
        glBindTexture(GL_TEXTURE_1D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        /* Unload texture */
        if (drc->modelTexture.texture) {
            glActiveTexture(GL_TEXTURE0 + drc->modelTexture.texture->textureId);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
 
    }

    if (showWireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }    
}

void DiffuseShader::setCells(unsigned int in) {
    numCells = glm::min(in, (unsigned int)16);
    cellIntensities.resize(numCells, 0.f);
    cellScales.resize(numCells, 0.f);
    for (int i = 0; i < numCells; i++) {
        float scale = 1.f - i / (float)numCells;
        cellIntensities[i] = (scale - 0.5) * 2;
        cellScales[i] = scale;
    }
}

void DiffuseShader::setCellIntensity(unsigned int i, float f) {
    cellIntensities[i] = (i == 0) ? f : glm::min(cellIntensities[i - 1], f);
}

void DiffuseShader::setCellScale(unsigned int i, float f) {
    cellScales[i] = (i == 0) ? f : glm::min(cellScales[i - 1], f);
}