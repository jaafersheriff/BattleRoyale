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
    dir = m_spatial.u() * uw.x + glm::vec3(0.0f, 1.0f, 0.0f) * dir.y +  m_spatial.w() * uw.y; // WoW controls
    m_spatial.move(dir * m_moveSpeed * dt);
}

void CameraController::lookAround(float dt) {
    m_camera.angle(float(Mouse::dx) * m_lookSpeed * dt, -float(Mouse::dy) * m_lookSpeed * dt, true);
}

void CameraController::moveFront(float dt) {
    m_spatial.move(-m_spatial.w() * m_moveSpeed * dt);
}

void CameraController::strafeForward(float dt) {
    m_spatial.move(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_spatial.u()) * m_moveSpeed * dt);
}

void CameraController::moveBack(float dt) {
    m_spatial.move(m_spatial.w() * m_moveSpeed * dt);
}

void CameraController::strafeBackward(float dt) {    
    m_spatial.move(glm::cross(m_spatial.u(), glm::vec3(0.0f, 1.0f, 0.0f)) * m_moveSpeed * dt);
}

void CameraController::moveRight(float dt) {
    m_spatial.move(m_spatial.u() * m_moveSpeed * dt);
}

void CameraController::moveLeft(float dt) {
    m_spatial.move(-m_spatial.u() * m_moveSpeed * dt);
}

void CameraController::moveUp(float dt) {
    m_spatial.move(glm::vec3(0.0f, 1.0f, 0.0f) * m_moveSpeed * dt);
}

void CameraController::moveDown(float dt) {
    m_spatial.move(glm::vec3(0.0f, -1.0f, 0.0f) * m_moveSpeed * dt);
}
