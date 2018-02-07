#include "DiffuseShader.hpp"

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

    for (auto cp : *components) {
        // TODO attributes, M, texture stuff
    }
}