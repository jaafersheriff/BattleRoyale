#include "DiffuseShader.hpp"
#include "Component/RenderComponents/DiffuseRenderComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

#include "glm/gtc/matrix_transform.hpp"

// Debug
#include "GameObject/GameObject.hpp"

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

    addUniform("lightPos");

    addUniform("matAmbient");
    addUniform("matDiffuse");
    addUniform("textureImage");
    addUniform("usesTexture");

    return true;
}

void DiffuseShader::render(const std::string & name, const std::vector<Component *> & components) {
    /* Bind uniforms */
    loadMat4(getUniform("P"), camera->getProj());
    loadMat4(getUniform("V"), camera->getView());
    loadVec3(getUniform("lightPos"), *lightPos);

    // Debug
    unsigned renderCount;
    renderCount = 0;

    /* Get data about the camera */
    // glm::vec3 pos = camera->getGameObject()->getSpatial()->position();
    // printf("Camera location: [%f, %f, %f]\n", pos[0], pos[1], pos[2]);

    for (auto cp : components) {
        // TODO : component list should be passed in as diffuserendercomponent
        DiffuseRenderComponent *drc;
        if (!(drc = dynamic_cast<DiffuseRenderComponent *>(cp)) || drc->pid != this->pid) {
            continue;
        }

        /* Model matrix */
        loadMat4(getUniform("M"), drc->getGameObject()->getSpatial()->modelMatrix());
        /* Normal matrix */
        loadMat3(getUniform("N"), drc->getGameObject()->getSpatial()->normalMatrix());

        /* Bind materials */
        loadFloat(getUniform("matAmbient"), drc->modelTexture.material.ambient);
        loadVec3(getUniform("matDiffuse"), drc->modelTexture.material.diffuse);
   
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
        glDrawElements(GL_TRIANGLES, (int)drc->mesh->eleBuf.size(), GL_UNSIGNED_INT, nullptr);
        // Debug
        renderCount = renderCount + 1;

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
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        /* Unload texture */
        if (drc->modelTexture.texture) {
            glActiveTexture(GL_TEXTURE0 + drc->modelTexture.texture->textureId);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    // Debug
    // printf("Rendered %u DiffuseRenderComponents\n", renderCount);
}