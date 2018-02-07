#include "DiffuseShader.hpp"

DiffuseShader::DiffuseShader(std::string vert, std::string frag, glm::vec3 *camera, glm::vec3 *light) :
    Shader(vert, frag) {
    /* Set global uniforms */
    this->cameraPos = camera;
    this->lightPos = light;
    /* Add attributes */
    addAttribute("vertPos");
    addAttribute("vertNor");
    addAttribute("vertTex");

    /* Add uniforms */
    addUniform("cameraPos");
    addUniform("lightPos");

    addUniform("matAmbient");
    addUniform("matDiffuse");
    addUniform("textureImage");
    addUniform("usesTexture");
}

void DiffuseShader::render(std::string name, std::vector<Component *> *components) {
    /* Bind uniforms */
    loadVec3(getUniform("cameraPos"), *this->cameraPos);
    loadVec3(getUniform("lightPos"), *this->lightPos);

    for (auto cp : *components) {
        // TODO
    }
}