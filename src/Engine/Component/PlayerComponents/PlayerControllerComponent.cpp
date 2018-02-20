#include "PlayerControllerComponent.hpp"

#include "IO/Window.hpp"
#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/SpatialComponents/PhysicsComponents.hpp"
#include "Component/PostCollisionComponents/GroundComponent.hpp"
#include "System/SpatialSystem.hpp"



PlayerControllerComponent::PlayerControllerComponent(float lookSpeed, float moveSpeed) :
    m_spatial(nullptr),
    m_newtonian(nullptr),
    m_camera(nullptr),
    m_lookSpeed(lookSpeed),
    m_moveSpeed(moveSpeed),
    m_enabled(true)
{}

void PlayerControllerComponent::init(GameObject & go) {
    Component::init(go);
    if (!(m_spatial = gameObject()->getComponentByType<SpatialComponent>())) assert(false);
    if (!(m_newtonian = gameObject()->getComponentByType<NewtonianComponent>())) assert(false);
    if (!(m_ground = gameObject()->getComponentByType<GroundComponent>())) assert(false);
    if (!(m_camera = gameObject()->getComponentByType<CameraComponent>())) assert(false);
}

void PlayerControllerComponent::update(float dt) {
    if (!m_enabled) {
        return;
    }
    if (Window::isImGuiEnabled()) {
        return;
    }

    // rotate camera
    if (Mouse::dx || Mouse::dy) {
        // angle head
        m_camera->angle(-float(Mouse::dx) * m_lookSpeed * dt, float(Mouse::dy) * m_lookSpeed * dt, true);
        // set body to that angle. this also angles head more as its orientation is relative to body
        m_spatial->setUVW(m_camera->u(), glm::vec3(0.0f, 1.0f, 0.0f), glm::cross(m_camera->u(), glm::vec3(0.0f, 1.0f, 0.0f)), true);
        // reset head to face forward. in absolute space, this puts it back to where it was before the last line
        m_camera->angle(0.0f, m_camera->phi(), false, true);
    }

    int forward(Keyboard::isKeyPressed(GLFW_KEY_W));
    int backward(Keyboard::isKeyPressed(GLFW_KEY_S));
    int left(Keyboard::isKeyPressed(GLFW_KEY_A));
    int right(Keyboard::isKeyPressed(GLFW_KEY_D));
    
    // move player
    glm::vec2 xzDir(
        float(right - left),
        float(backward - forward)
    );
    if (xzDir != glm::vec2()) {
        xzDir = glm::normalize(xzDir);
        // would-be direction of movement on the xz plane
        glm::vec3 dir = xzDir.x * m_spatial->u() + xzDir.y * m_spatial->w();
        // take into account slope of ground
        // TODO: if for whatever reason we want the player to be able to move
        // on the ground when the ground is >= 90 degrees off the xz plane, this
        // will not work
        if (m_ground->onGround()) {
            dir = glm::normalize(Util::projectOnto(dir, m_ground->groundNorm()));
        }
        m_spatial->move(dir * m_moveSpeed * dt);
        // remove some velocity against direction of movement
        m_newtonian->removeSomeVelocityAgainstDir(dir, m_moveSpeed);
    }

    // jump
    if(Keyboard::isKeyPressed(GLFW_KEY_SPACE)) {
        if (m_ground->onGround()) gameObject()->getComponentByType<NewtonianComponent>()->addVelocity(-3.0f * glm::normalize(SpatialSystem::get().gravity()));
    }
}

void PlayerControllerComponent::setEnabled(bool enabled) {
    m_enabled = enabled;
}