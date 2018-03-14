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
    addAttribute("orientationID");

    /* Add uniforms */
    addUniform("P");
    addUniform("M");
    addUniform("randomMs");
    addUniform("V");
    addUniform("N");
    addUniform("randomNs");
    addUniform("randomOrientation");

    addUniform("lightDir");
    addUniform("camPos");

    addUniform("matAmbient");
    addUniform("matDiffuse");
    addUniform("matSpecular");
    addUniform("shine");
    addUniform("textureImage");
    addUniform("usesTexture");

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

    for (ParticleComponent * pc : ParticleSystem::s_particleComponents) {
        if (pc->getParticlePositions()->size() == 0) {
            break;
        }

        // The reason there is a plus one is that there has to always be at least one element in RandomMs even if unused.
        if (pc->RandomMs().size() > pc->MAX_ORIENTATIONS + 1) {
            break;
        }
        bool r = (int)pc->RandomMs().size() != 1;
        loadBool(getUniform("randomOrientation"), r);

        /* Model matrix */
        
        /* Load Random Matrix Array */
        loadMat4(getUniform("M"), pc->ModelMatrix());
        loadMultiMat4(getUniform("randomMs"), &(pc->RandomMs()[0]), (int)pc->RandomMs().size());
        
        /* Load Normal matrix */
        loadMat3(getUniform("N"), pc->NormalMatrix());
        loadMultiMat3(getUniform("randomNs"), &(pc->RandomNs()[0]), (int)pc->RandomNs().size());



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
        Vector<glm::vec3> *positions = pc->getParticlePositions();

        /* Do all the buffer stuff */
        glGenBuffers(1, &offsetBufId);
        glBindBuffer(GL_ARRAY_BUFFER, offsetBufId);
        
        glBufferData(GL_ARRAY_BUFFER, positions->size() * sizeof(glm::vec3), &(*positions)[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(posPO);
        glVertexAttribPointer(posPO, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        /* Update particle position attribute once per instance */
        glVertexAttribDivisor(posPO, 1);


        int randOrient = getAttribute("orientationID");
        unsigned int orientBufId;
        Vector<int> *orientations = pc->getParticleOrientationIDs();

        /* Do all the buffer stuff */
        glGenBuffers(1, &orientBufId);
        glBindBuffer(GL_ARRAY_BUFFER, orientBufId);
        glBufferData(GL_ARRAY_BUFFER, orientations->size() * sizeof(int), &(*orientations)[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(randOrient);
        glVertexAttribPointer(randOrient, 1, GL_INT, GL_FALSE, 0, (const void *)0);
        glVertexAttribDivisor(randOrient, 1);


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
        glDrawElementsInstanced(GL_TRIANGLES, (int)pc->getMesh(0)->eleBufSize, GL_UNSIGNED_INT, nullptr, (GLsizei)positions->size());

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