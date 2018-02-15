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
    camera->angle(float(Mouse::dx) * lookSpeed * dt, -float(Mouse::dy) * lookSpeed * dt, true);
}

void CameraController::moveFront(float dt) {
    gameObject->getSpatial()->move(-camera->w() * moveSpeed * dt);
}

void CameraController::strafeForward(float dt) {
    gameObject->getSpatial()->move(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), camera->u()) * moveSpeed * dt);
}

void CameraController::moveBack(float dt) {
    gameObject->getSpatial()->move(camera->w() * moveSpeed * dt);
}

void CameraController::strafeBackward(float dt) {    
    gameObject->getSpatial()->move(glm::cross(camera->u(), glm::vec3(0.0f, 1.0f, 0.0f)) * moveSpeed * dt);
}

void CameraController::moveRight(float dt) {
    gameObject->getSpatial()->move(camera->u() * moveSpeed * dt);
}

void CameraController::moveLeft(float dt) {
    gameObject->getSpatial()->move(-camera->u() * moveSpeed * dt);
}

void CameraController::moveUp(float dt) {
    gameObject->getSpatial()->move(glm::vec3(0.0f, 1.0f, 0.0f) * moveSpeed * dt);
}

void CameraController::moveDown(float dt) {
    gameObject->getSpatial()->move(glm::vec3(0.0f, -1.0f, 0.0f) * moveSpeed * dt);
}
