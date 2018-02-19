#include "PlayerControllerComponent.hpp"

#include "IO/Window.hpp"
#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/SpatialComponents/PhysicsComponents.hpp"



PlayerControllerComponent::PlayerControllerComponent(float lookSpeed, float moveSpeed) :
    m_spatial(nullptr),
    m_newtonian(nullptr),
    m_camera(nullptr),
    m_lookSpeed(lookSpeed),
    m_moveSpeed(moveSpeed),
    m_enabled(true)
{}

void PlayerControllerComponent::init(GameObject & gameObject) {
    m_gameObject = &gameObject;
    if (!(m_spatial = m_gameObject->getComponentByType<SpatialComponent>())) assert(false);
    if (!(m_newtonian = m_gameObject->getComponentByType<NewtonianComponent>())) assert(false);
    if (!(m_camera = m_gameObject->getComponentByType<CameraComponent>())) assert(false);
}

void PlayerControllerComponent::update(float dt) {
    if (!m_enabled) {
        return;
    }
    if (Window::isImGuiEnabled()) {
        return;
    }

    if (Mouse::dx || Mouse::dy) {
        // angle head
        m_camera->angle(-float(Mouse::dx) * m_lookSpeed * dt, float(Mouse::dy) * m_lookSpeed * dt, true);
        // set body to that angle. this also angles head more
        m_spatial->setUVW(m_camera->u(), glm::vec3(0.0f, 1.0f, 0.0f), glm::cross(m_camera->u(), glm::vec3(0.0f, 1.0f, 0.0f)), true);
        // reset head to face forward. in absolute space, this puts it back to where it was before the last line
        m_camera->angle(0.0f, m_camera->phi(), false, true);
    }

    int forward(Keyboard::isKeyPressed(GLFW_KEY_W));
    int backward(Keyboard::isKeyPressed(GLFW_KEY_S));
    int left(Keyboard::isKeyPressed(GLFW_KEY_A));
    int right(Keyboard::isKeyPressed(GLFW_KEY_D));
    
    glm::vec2 xzDir(
        float(right - left),
        float(backward - forward)
    );
    if (xzDir != glm::vec2()) {
        xzDir = glm::normalize(xzDir);
        glm::vec3 dir = xzDir.x * m_spatial->u() + xzDir.y * m_spatial->w();
        m_spatial->move(dir * m_moveSpeed * dt);
    }

    if(Keyboard::isKeyPressed(GLFW_KEY_SPACE)) {
        m_gameObject->getComponentByType<NewtonianComponent>()->accelerate(glm::vec3(10.0f, 0.0f, 0.0f));
    }
}

void PlayerControllerComponent::setEnabled(bool enabled) {
    m_enabled = enabled;
}