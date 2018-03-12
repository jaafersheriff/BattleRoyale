#include "ParticleShader.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/ParticleComponents/ParticleComponent.hpp"
#include "System/CollisionSystem.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"

ParticleShader::ParticleShader(const String & vertFile, const String & fragFile, const glm::vec3 & light) :
    Shader(vertFile, fragFile),
    lightDir(&light) {
    //cellIntensities.resize(1, 1.f);
    //cellScales.resize(1, 1.f);
}

bool ParticleShader::init() {
    if (!Shader::init()) {
        return false;
    }
    /* Add attributes */
    addAttribute("vertPos");
    addAttribute("vertNor");
    addAttribute("vertTex");
    addAttribute("particleOffset");

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

    /*addUniform("isToon");
    addUniform("silAngle");
    addUniform("numCells");
    addUniform("cellIntensities");
    addUniform("cellScales");
    */
    /* Generate 1D Textures with initial size of 16 floats */
   /* glActiveTexture(GL_TEXTURE0);
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
    */
    return true;
}

void ParticleShader::render(const CameraComponent * camera, const Vector<Component *> & components) {
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
    loadVec3(getUniform("camPos"), camera->gameObject().getSpatial()->position());

    /* Toon shading */
    /*loadFloat(getUniform("silAngle"), silAngle);
    loadFloat(getUniform("numCells"), (float)numCells);
    loadInt(getUniform("cellIntensities"), cellIntensitiesTexture);
    glActiveTexture(GL_TEXTURE0 + cellIntensitiesTexture);
    glBindTexture(GL_TEXTURE_1D, cellIntensitiesTexture);
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, int(cellIntensities.size()), GL_RED, GL_FLOAT, cellIntensities.data());
    loadInt(getUniform("cellScales"), cellScalesTexture);
    glActiveTexture(GL_TEXTURE0 + cellScalesTexture);
    glBindTexture(GL_TEXTURE_1D, cellScalesTexture);
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, int(cellScales.size()), GL_RED, GL_FLOAT, cellScales.data());
    */
    for (ParticleComponent * pc : ParticleSystem::s_particleComponents) {
        // TODO : component list should be passed in as ParticleRenderComponent
        //ParticleRenderComponent *prc;
        /*if (!(prc = dynamic_cast<ParticleRenderComponent *>(comp)) || prc->pid != this->pid) {
            continue;
        }*/

        //ParticleComponent & pc = prc->pc;
        //loadBool(getUniform("isToon"), false);



        //ISSUE HERE - NO SPATIAL COMPONENT
        /* Model matrix */
        loadMat4(getUniform("M"), pc->ModelMatrix());
        /* Normal matrix */
        loadMat3(getUniform("N"), pc->NormalMatrix());



        /* Bind materials */
        loadFloat(getUniform("matAmbient"), pc->getModelTexture(0)->material.ambient);
        loadVec3(getUniform("matDiffuse"), pc->getModelTexture(0)->material.diffuse);
        loadVec3(getUniform("matSpecular"), pc->getModelTexture(0)->material.specular);
        loadFloat(getUniform("shine"), pc->getModelTexture(0)->material.shineDamper);
   
        /* Load texture */
        if(pc->getModelTexture(0)->texture && pc->getModelTexture(0)->texture->textureId != 0) {
            loadBool(getUniform("usesTexture"), true);
            loadInt(getUniform("textureImage"), pc->getModelTexture(0)->texture->textureId);
            glActiveTexture(GL_TEXTURE0 + pc->getModelTexture(0)->texture->textureId);
            glBindTexture(GL_TEXTURE_2D, pc->getModelTexture(0)->texture->textureId);
        }
        else {
            loadBool(getUniform("usesTexture"), false);
        }

       


        /* Bind mesh */
        glBindVertexArray(pc->getMesh(0)->vaoId);
            
        /*Set Particle positions*/
        int posPO = getAttribute("particleOffset");
        unsigned int offsetBufId;
        Vector<float> offsetBuf;
        Vector<glm::vec3> *positions = pc->getParticlePositions();
        for (int i = 0; i < pc->Count(); i++) {
            offsetBuf.push_back((*positions)[i].x);
            offsetBuf.push_back((*positions)[i].y);
            offsetBuf.push_back((*positions)[i].z);
        }

        /* Do all the buffer stuff */
        glGenBuffers(1, &offsetBufId);
        glBindBuffer(GL_ARRAY_BUFFER, offsetBufId);
        
        //glBufferData(GL_ARRAY_BUFFER, offsetBuf.size() * sizeof(float), &offsetBuf[0], GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, offsetBuf.size() * sizeof(float), &(*positions)[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(posPO);
        glVertexAttribPointer(posPO, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        /* Update particle position attribute once per instance */
        glVertexAttribDivisor(posPO, 1);

        /* Bind vertex buffer VBO */
        int pos = getAttribute("vertPos");
        glEnableVertexAttribArray(pos);
        glBindBuffer(GL_ARRAY_BUFFER, pc->getMesh(0)->vertBufId);
        glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        /* Bind normal buffer VBO */
        pos = getAttribute("vertNor");
        if (pos != -1 && pc->getMesh(0)->norBufId != 0) {
            glEnableVertexAttribArray(pos);
            glBindBuffer(GL_ARRAY_BUFFER, pc->getMesh(0)->norBufId);
            glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        /* Bind texture coordinate buffer VBO */
        pos = getAttribute("vertTex");
        if (pos != -1 && pc->getMesh(0)->texBufId != 0) {
            glEnableVertexAttribArray(pos);
            glBindBuffer(GL_ARRAY_BUFFER, pc->getMesh(0)->texBufId);
            glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        /* Bind indices buffer VBO */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pc->getMesh(0)->eleBufId);

        /* DRAW */
        //glDrawElements(GL_TRIANGLES, (int)pc->getMesh(0)->eleBufSize, GL_UNSIGNED_INT, nullptr);
        glDrawElementsInstanced(GL_TRIANGLES, (int)pc->getMesh(0)->eleBufSize, GL_UNSIGNED_INT, nullptr, pc->Count());

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
        if (pc->getModelTexture(0)->texture) {
            glActiveTexture(GL_TEXTURE0 + pc->getModelTexture(0)->texture->textureId);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
 
    }

    if (showWireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }    
}

/*void ParticleShader::setCells(unsigned int in) {
    numCells = glm::min(in, (unsigned int)16);
    cellIntensities.resize(numCells, 0.f);
    cellScales.resize(numCells, 0.f);
    for (int i = 0; i < int(numCells); i++) {
        float scale = 1.f - i / (float)numCells;
        cellIntensities[i] = (scale - 0.5f) * 2.0f;
        cellScales[i] = scale;
    }
}

void ParticleShader::setCellIntensity(unsigned int i, float f) {
    cellIntensities[i] = (i == 0) ? f : glm::min(cellIntensities[i - 1], f);
}

void ParticleShader::setCellScale(unsigned int i, float f) {
    cellScales[i] = (i == 0) ? f : glm::min(cellScales[i - 1], f);
}*/