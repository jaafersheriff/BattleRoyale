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

    int forward(Keyboard::isKeyPressed(GLFW_KEY_W));
    int backward(Keyboard::isKeyPressed(GLFW_KEY_S));
    int left(Keyboard::isKeyPressed(GLFW_KEY_A));
    int right(Keyboard::isKeyPressed(GLFW_KEY_D));
    int up(Keyboard::isKeyPressed(GLFW_KEY_SPACE));
    int down(Keyboard::isKeyPressed(GLFW_KEY_LEFT_SHIFT));
    
    glm::vec3 dir(
        float(right - left),
        float(up - down),
        float(backward - forward)
    );
    if (dir != glm::vec3())
        dir = glm::normalize(dir);

    glm::vec2 uw(dir.x, dir.z);
    dir = camera->u() * uw.x + glm::vec3(0.0f, 1.0f, 0.0f) * dir.y +  camera->w() * uw.y; // WoW controls
    gameObject->getSpatial()->move(dir * moveSpeed * dt);
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
