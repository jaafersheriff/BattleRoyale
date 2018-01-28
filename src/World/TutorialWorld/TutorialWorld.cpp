#include "TutorialWorld.hpp"
#include "glm/gtc/matrix_transform.hpp"

void TutorialWorld::init(Context &ctx, Loader &loader) {
    /* Set up light */
    this->light = new Light(glm::vec3(-1000, 1000, 1000), glm::vec3(1.f), glm::vec3(1.f, 0.0f, 0.0f));

    /* Set up camera */
    this->camera = new Camera;

    /* Entities */
    Mesh *mesh = loader.loadObjMesh("sword.obj");
    ModelTexture modelTexture(0.3f,
                    glm::vec3(0.f, 0.f, 1.f),
                    glm::vec3(1.f));
    //Where I'm adding all of my models
    entities.push_back(new Enemy(mesh, modelTexture, glm::vec3(5.f, 0.f, 0.f), glm::vec3(0.f), glm::vec3(3.f)));
    entities.push_back(new Enemy(mesh, modelTexture, glm::vec3(16.f, 30.f, 29.f), glm::vec3(0.f), glm::vec3(3.f)));
    entities.push_back(new Enemy(mesh, modelTexture, glm::vec3(17.f, 19.f, 27.f), glm::vec3(0.f), glm::vec3(3.f)));
    entities.push_back(new Enemy(mesh, modelTexture, glm::vec3(30.f, 29.f, 14.f), glm::vec3(0.f), glm::vec3(3.f)));
    entities.push_back(new Enemy(mesh, modelTexture, glm::vec3(17.f, 4.f, 17.f), glm::vec3(0.f), glm::vec3(3.f)));

    /* World-specific members */
    this->P = ctx.display.projectionMatrix;
    this->V = glm::lookAt(camera->position, camera->lookAt, glm::vec3(0, 1, 0));
}

void TutorialWorld::prepareRenderer(MasterRenderer *mr) {
    mr->activateEntityShader(&entities);
}

void TutorialWorld::prepareUniforms() {
    UniformData *PData = new UniformData{ UniformType::Mat4, "P", (void *)&P };
    UniformData *VData = new UniformData{ UniformType::Mat4, "V", (void *)&V };
    UniformData *cameraPos = new UniformData{ UniformType::Vec3, "cameraPos", (void *)&camera->position };
    UniformData *lightPos = new UniformData{ UniformType::Vec3, "lightPos", (void *)&light->position };
    UniformData *lightCol = new UniformData{ UniformType::Vec3, "lightCol", (void *)&light->color };
    UniformData *lightAtt = new UniformData{ UniformType::Vec3, "lightAtt", (void *)&light->attenuation };
    
    uniforms[MasterRenderer::ShaderTypes::ENTITY_SHADER].push_back(PData);
    uniforms[MasterRenderer::ShaderTypes::ENTITY_SHADER].push_back(VData);
    uniforms[MasterRenderer::ShaderTypes::ENTITY_SHADER].push_back(cameraPos);
    uniforms[MasterRenderer::ShaderTypes::ENTITY_SHADER].push_back(lightPos);
    uniforms[MasterRenderer::ShaderTypes::ENTITY_SHADER].push_back(lightCol);
    uniforms[MasterRenderer::ShaderTypes::ENTITY_SHADER].push_back(lightAtt);
}

void TutorialWorld::update(Context &ctx) {
    this->P = ctx.display.projectionMatrix;
    this->V = glm::lookAt(camera->position, camera->lookAt, glm::vec3(0, 1, 0));
    takeInput(ctx.mouse, ctx.keyboard);
    camera->update();
  
    /* Update entities */
    for (auto e : entities) {
        ((Enemy *)e)->playerPos = this->camera->position;
        ((Enemy *)e)->update(entities);
        e->rotation.y += 15.f * ctx.timeStep;
    }
}

void TutorialWorld::takeInput(Mouse &mouse, Keyboard &keyboard) {
    if (keyboard.isKeyPressed(' ')) {
        isPaused = !isPaused;
    }

    if (mouse.isButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        camera->takeMouseInput(mouse.dx, mouse.dy);
    }
    else {
        camera->takeMouseInput(0.f, 0.f);
    }
    if (keyboard.isKeyPressed('w')) {
        camera->moveForward();
    }
    if (keyboard.isKeyPressed('a')) {
        camera->moveLeft();
    }
    if (keyboard.isKeyPressed('s')) {
        camera->moveBackward();
    }
    if (keyboard.isKeyPressed('d')) {
        camera->moveRight();
    }
    if (keyboard.isKeyPressed('e')) {
        camera->moveDown();
    }
    if (keyboard.isKeyPressed('r')) {
        camera->moveUp();
    }
}

void TutorialWorld::cleanUp() {
    
}

