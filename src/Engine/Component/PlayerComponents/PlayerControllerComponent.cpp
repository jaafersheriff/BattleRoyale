#include "PlayerControllerComponent.hpp"

#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"



PlayerControllerComponent::PlayerControllerComponent(CameraComponent & camera, float lookSpeed, float moveSpeed) :
    m_spatial(*camera.getGameObject()->getSpatial()),
    m_camera(camera),
    m_lookSpeed(lookSpeed),
    m_moveSpeed(moveSpeed),
    m_enabled(true)
{}

void PlayerControllerComponent::update(float dt) {
    if (!m_enabled) {
        return;
    }

    if (Mouse::dx || Mouse::dy) {
        m_camera.angle(float(Mouse::dx) * m_lookSpeed * dt, -float(Mouse::dy) * m_lookSpeed * dt, true);
    }

    int forward(Keyboard::isKeyPressed(GLFW_KEY_W));
    int backward(Keyboard::isKeyPressed(GLFW_KEY_S));
    int left(Keyboard::isKeyPressed(GLFW_KEY_A));
    int right(Keyboard::isKeyPressed(GLFW_KEY_D));
    
    glm::vec2 xzDir(
        float(right - left),
        float(backward - forward)
    );
    if (xzDir != glm::vec2())
        xzDir = glm::normalize(xzDir);

    glm::vec3 dir = xzDir.x * m_spatial.u() + xzDir.y * m_spatial.w();
    m_spatial.move(dir * m_moveSpeed * dt);

    if(Keyboard::isKeyPressed(GLFW_KEY_SPACE)) {
        
    }
}

void PlayerControllerComponent::setEnabled(bool enabled) {
    m_enabled = enabled;
}