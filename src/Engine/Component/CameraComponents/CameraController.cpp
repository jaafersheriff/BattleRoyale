#include "CameraController.hpp"

#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"

void CameraController::init() {

}

void CameraController::update(float dt) {
    if (Mouse::isDown(GLFW_MOUSE_BUTTON_1)) {
        lookAround(dt);
    }

    if (Keyboard::isKeyPressed(front)) {
        moveFront(dt);
    }
    if (Keyboard::isKeyPressed(back)) {
        moveBack(dt);
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
}

void CameraController::moveFront(float dt) {
    gameObject->transform.position += camera->w * moveSpeed * dt;
    camera->lookAt += camera->w * moveSpeed * dt;
}

void CameraController::moveBack(float dt) {
    gameObject->transform.position -= camera->w * moveSpeed * dt;
    camera->lookAt -= camera->w * moveSpeed * dt;
}

void CameraController::moveRight(float dt) {
    gameObject->transform.position += camera->u * moveSpeed * dt;
    camera->lookAt += camera->u * moveSpeed * dt;
}

void CameraController::moveLeft(float dt) {
    gameObject->transform.position -= camera->u * moveSpeed * dt;
    camera->lookAt -= camera->u * moveSpeed * dt;
}

void CameraController::moveUp(float dt) {
    gameObject->transform.position += camera->v * moveSpeed * dt;
    camera->lookAt += camera->v * moveSpeed * dt;
}

void CameraController::moveDown(float dt) {
    gameObject->transform.position -= camera->v * moveSpeed * dt;
    camera->lookAt -= camera->v * moveSpeed * dt;
}

