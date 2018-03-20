#include "CameraControllerComponent.hpp"

#include "IO/Window.hpp"
#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "CameraComponent.hpp"
#include "GameObject/Message.hpp"
#include "Scene/Scene.hpp"
#include "Util/Util.hpp"

CameraControllerComponent::CameraControllerComponent(GameObject & gameObject, float lookSpeed, float minMoveSpeed, float maxMoveSpeed) :
    Component(gameObject),
    m_spatial(nullptr),
    m_theta(0.0f), m_phi(glm::pi<float>() * 0.5f),
    m_lookSpeed(lookSpeed),
    m_minMoveSpeed(glm::max(minMoveSpeed, 0.0f)),
    m_maxMoveSpeed(glm::max(maxMoveSpeed, m_minMoveSpeed)),
    m_moveSpeed(glm::mix(m_minMoveSpeed, m_maxMoveSpeed, 0.5f)),
    m_enabled(true)
{}

void CameraControllerComponent::init() {
    if (!(m_spatial = gameObject().getSpatial())) assert(false);

    auto scrollCallback([&](const Message & msg_) {
        static float s_percentage = 0.5f;

        const ScrollMessage & msg(static_cast<const ScrollMessage &>(msg_));
        if (!m_enabled) {
            return;
        }
        if (m_minMoveSpeed == m_maxMoveSpeed) {
            return;
        }
        s_percentage = glm::clamp(s_percentage + msg.dy * 0.1f, 0.0f, 1.0f);
        m_moveSpeed = glm::mix(m_minMoveSpeed, m_maxMoveSpeed, s_percentage * s_percentage);
    });
    Scene::addReceiver<ScrollMessage>(nullptr, scrollCallback);
}

void CameraControllerComponent::update(float dt) {
    if (!m_enabled) {
        return;
    }

    if (Mouse::dx || Mouse::dy) {
        m_theta -= float(Mouse::dx) * m_lookSpeed;
        m_phi += float(Mouse::dy) * m_lookSpeed;
        updateSpatialOrientation();
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
        dir = Util::safeNorm(dir);
        dir = Util::safeNorm(m_spatial->u() * dir.x + glm::vec3(0.0f, 1.0f, 0.0f) * dir.y + m_spatial->w() * dir.z); // WoW controls
        m_spatial->move(dir * m_moveSpeed * dt);
    }
}

void CameraControllerComponent::setOrientation(float theta, float phi) {
    m_theta = theta;
    m_phi = phi;
    updateSpatialOrientation();
}

void CameraControllerComponent::setEnabled(bool enabled) {
    m_enabled = enabled;
}

void CameraControllerComponent::updateSpatialOrientation() {    
    if      (m_theta >  glm::pi<float>()) m_theta = std::fmod(m_theta, glm::pi<float>()) - glm::pi<float>();
    else if (m_theta < -glm::pi<float>()) m_theta = glm::pi<float>() - std::fmod(-m_theta, glm::pi<float>());
    if (m_phi < 0.0f) m_phi = 0.0f;
    if (m_phi > glm::pi<float>()) m_phi = glm::pi<float>();

    glm::vec3 w(-Util::sphericalToCartesian(1.0f, m_theta, m_phi));
    w = glm::vec3(-w.y, w.z, -w.x); // one of the many reasons I like z to be up
    glm::vec3 v(Util::sphericalToCartesian(1.0f, m_theta, m_phi - glm::pi<float>() * 0.5f));
    v = glm::vec3(-v.y, v.z, -v.x);
    glm::vec3 u(glm::cross(v, w));
    m_spatial->setRelativeUVW(u, v, w);
}
