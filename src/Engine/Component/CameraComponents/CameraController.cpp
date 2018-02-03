#include "CameraController.hpp"

#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"

void CameraController::init() {

}

void CameraController::update(float dt) {
    if (Mouse::isDown(0)) {
        lookAround(dt, Mouse::dx, Mouse::dy);
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

void CameraController::lookAround(float dt, float dx, float dy) {
    camera->updateTheta(dx * lookSpeed * dt);
    camera->updatePhi(dy * lookSpeed * dt);
}

void CameraController::moveFront(float dt) {
    camera->position += camera->w * moveSpeed * dt;
    camera->lookAt += camera->w * moveSpeed * dt;
}

void CameraController::moveBack(float dt) {
    camera->position -= camera->w * moveSpeed * dt;
    camera->lookAt -= camera->w * moveSpeed * dt;
}

void CameraController::moveRight(float dt) {
    camera->position += camera->u * moveSpeed * dt;
    camera->lookAt += camera->u * moveSpeed * dt;
}

void CameraController::moveLeft(float dt) {
    camera->position -= camera->u * moveSpeed * dt;
    camera->lookAt -= camera->u * moveSpeed * dt;
}

void CameraController::moveUp(float dt) {
    camera->position += camera->v * moveSpeed * dt;
    camera->lookAt += camera->v * moveSpeed * dt;
}

void CameraController::moveDown(float dt) {
    camera->position -= camera->v * moveSpeed * dt;
    camera->lookAt -= camera->v * moveSpeed * dt;
}

