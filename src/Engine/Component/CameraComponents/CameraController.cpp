#include "CameraController.hpp"

#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

void CameraController::init() {

}

void CameraController::update(float dt) {
    if (Mouse::isDown(GLFW_MOUSE_BUTTON_1)) {
        lookAround(dt);
    }

    if (Keyboard::isKeyPressed(front)) {
        //moveFront(dt);
        strafeForward(dt);
    }
    if (Keyboard::isKeyPressed(back)) {
        //moveBack(dt);
        strafeBackward(dt);
    }
    if (Keyboard::isKeyPressed(left)) {
        moveLeft(dt);
    }
    if (Keyboard::isKeyPressed(right)) {
        moveRight(dt);
    }
    if (Keyboard::isKeyPressed(up)) {
        moveUp(dt);
    }
    if (Keyboard::isKeyPressed(down)) {
        moveDown(dt);
    }
}
void CameraController::lookAround(float dt) {
    camera->theta += Mouse::dx * lookSpeed * dt;
    camera->phi -= Mouse::dy * lookSpeed * dt;
    camera->setDirty();
}

void CameraController::moveFront(float dt) {
    gameObject->getSpatial()->move(camera->w * moveSpeed * dt);
    camera->lookAt += camera->w * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::strafeForward(float dt) {
    glm::vec3 view = camera->getGameObject()->getSpatial()->position() - camera->lookAt;
    glm::vec3 sideDir = glm::cross(view, glm::vec3(0, 1, 0));
    glm::vec3 forwardDir = glm::cross(sideDir, glm::vec3(0, 1, 0));

    gameObject->getSpatial()->move(forwardDir * moveSpeed * dt);
    camera->lookAt += forwardDir * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::moveBack(float dt) {
    gameObject->getSpatial()->move(-(camera->w * moveSpeed * dt));
    camera->lookAt -= camera->w * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::strafeBackward(float dt) {
    glm::vec3 view = camera->lookAt - camera->getGameObject()->getSpatial()->position();
    glm::vec3 sideDir = glm::cross(view, glm::vec3(0, 1, 0));
    glm::vec3 backwardDir = glm::cross(sideDir, glm::vec3(0, 1, 0));

    gameObject->getSpatial()->move(backwardDir * moveSpeed * dt);
    camera->lookAt += backwardDir * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::moveRight(float dt) {
    gameObject->getSpatial()->move(camera->u * moveSpeed * dt);
    camera->lookAt += camera->u * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::moveLeft(float dt) {
    gameObject->getSpatial()->move(-(camera->u * moveSpeed * dt));
    camera->lookAt -= camera->u * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::moveUp(float dt) {
    gameObject->getSpatial()->move(camera->v * moveSpeed * dt);
    camera->lookAt += camera->v * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::moveDown(float dt) {
    gameObject->getSpatial()->move(-(camera->v * moveSpeed * dt));
    camera->lookAt -= camera->v * moveSpeed * dt;
    camera->setDirty();
}
