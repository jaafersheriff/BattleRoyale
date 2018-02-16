#include "CameraControllerComponent.hpp"

#include "IO/Window.hpp"
#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "CameraComponent.hpp"

CameraControllerComponent::CameraControllerComponent(CameraComponent & cc, float ls, float ms) :
    m_spatial(*cc.getGameObject()->getSpatial()),
    m_camera(cc),
    m_lookSpeed(ls),
    m_moveSpeed(ms),
    m_enabled(true)
{}

void CameraControllerComponent::init() {

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
        m_camera.angle(-float(Mouse::dx) * m_lookSpeed * dt, float(Mouse::dy) * m_lookSpeed * dt, true);
        // set camera base to same orientation
        m_spatial.setUVW(m_camera.u(), m_camera.v(), m_camera.w(), true);
        // reset camera to face forward. in absolute space, this puts it back to where it was before the last line
        m_camera.angle(0.0f, glm::pi<float>() * 0.5f, false, true);
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
        dir = m_spatial.u() * xzDir.x + glm::vec3(0.0f, 1.0f, 0.0f) * dir.y +  m_spatial.w() * xzDir.y; // WoW controls
        m_spatial.move(dir * m_moveSpeed * dt);
    }
}

void CameraControllerComponent::setEnabled(bool enabled) {
    m_enabled = enabled;
}
