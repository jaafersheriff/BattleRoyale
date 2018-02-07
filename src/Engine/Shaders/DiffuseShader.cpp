#include "DiffuseShader.hpp"
#include "Component/RenderComponents/DiffuseRenderableComponent.hpp"

#include "glm/gtc/matrix_transform.hpp"

DiffuseShader::DiffuseShader(std::string vert, std::string frag, CameraComponent *cam, glm::vec3 *light) :
    Shader(vert, frag) {
    /* Set global uniforms */
    this->camera = cam;
    this->lightPos = light;

    /* Add attributes */
    addAttribute("vertPos");
    addAttribute("vertNor");
    addAttribute("vertTex");

    /* Add uniforms */
    addUniform("P");
    addUniform("M");
    addUniform("V");

    addUniform("cameraPos");
    addUniform("lightPos");

    addUniform("matAmbient");
    addUniform("matDiffuse");
    addUniform("textureImage");
    addUniform("usesTexture");
}

void DiffuseShader::render(std::string name, std::vector<Component *> *components) {
    /* Bind uniforms */
    loadMat4(getUniform("P"), &camera->getProj());
    loadMat4(getUniform("V"), &camera->getView());
    loadVec3(getUniform("cameraPos"), camera->getGameObject()->transform.position);
    loadVec3(getUniform("lightPos"), *lightPos);

    glm::mat4 M;
    for (auto cp : *components) {
        // TODO : component list should be passed in as diffuserenderablecomponent
        DiffuseRenderableComponent *drc;
        if (!(drc = dynamic_cast<DiffuseRenderableComponent *>(cp))) {
            continue;
        }

        /* Model matrix */
        M  = glm::mat4(1.f);
        M *= glm::rotate(glm::mat4(1.f), glm::radians(drc->getGameObject()->transform.rotation.x), glm::vec3(1, 0, 0));
        M *= glm::rotate(glm::mat4(1.f), glm::radians(drc->getGameObject()->transform.rotation.y), glm::vec3(0, 1, 0));
        M *= glm::rotate(glm::mat4(1.f), glm::radians(drc->getGameObject()->transform.rotation.z), glm::vec3(0, 0, 1));
        M *= glm::scale(glm::mat4(1.f), drc->getGameObject()->transform.scale);
        loadMat4(getUniform("M"), &M);

        /* Bind materials */
        loadFloat(getUniform("matAmbient"), drc->modelTexture.material.ambient);
        loadVec3(getUniform("matdiffuse"), drc->modelTexture.material.diffuse);
   
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
}