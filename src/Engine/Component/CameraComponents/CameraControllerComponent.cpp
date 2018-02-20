#include "CameraControllerComponent.hpp"

#include "IO/Window.hpp"
#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "CameraComponent.hpp"

CameraControllerComponent::CameraControllerComponent(float ls, float ms) :
    m_spatial(nullptr),
    m_camera(nullptr),
    m_lookSpeed(ls),
    m_moveSpeed(ms),
    m_enabled(true)
{}

void CameraControllerComponent::init(GameObject & go) {
    Component::init(go);
    if (!(m_spatial = gameObject()->getSpatial())) assert(false);
    if (!(m_camera = gameObject()->getComponentByType<CameraComponent>())) assert(false);
}

void CameraControllerComponent::update(float dt) {
    if (!m_enabled) {
        return;
    }
    if (Window::isImGuiEnabled()) {
        return;
    }

    if (Mouse::dx || Mouse::dy) {
        // orient camera relative to base
        m_camera->angle(-float(Mouse::dx) * m_lookSpeed * dt, float(Mouse::dy) * m_lookSpeed * dt, true);
        // set camera base to same xz orientation
        m_spatial->setUVW(m_camera->u(), glm::vec3(0.0f, 1.0f, 0.0f), glm::cross(m_camera->u(), glm::vec3(0.0f, 1.0f, 0.0f)), true);
        // reset camera to face forward. in absolute space, this puts it back to where it was before the last line
        m_camera->angle(0.0f, m_camera->phi(), false, true);
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
    if (dir != glm::vec3()) {
        dir = glm::normalize(dir);
        glm::vec2 xzDir(dir.x, dir.z);
        dir = m_spatial->u() * xzDir.x + glm::vec3(0.0f, 1.0f, 0.0f) * dir.y +  m_spatial->w() * xzDir.y; // WoW controls
        m_spatial->move(dir * m_moveSpeed * dt);
    }
}

void CameraControllerComponent::setEnabled(bool enabled) {
    m_enabled = enabled;
}
