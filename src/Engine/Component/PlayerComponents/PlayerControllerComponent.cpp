#include "PlayerControllerComponent.hpp"

#include "IO/Window.hpp"
#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/SpatialComponents/PhysicsComponents.hpp"
#include "Component/PostCollisionComponents/GroundComponent.hpp"
#include "System/SpatialSystem.hpp"
#include "System/SoundSystem.hpp"
#include "Util/Util.hpp"



PlayerControllerComponent::PlayerControllerComponent(GameObject & gameObject, float lookSpeed, float moveSpeed, float jumpSpeed, float sprintSpeed) :
    Component(gameObject),
    m_bodySpatial(nullptr),
    m_headSpatial(nullptr),
    m_newtonian(nullptr),
    m_theta(0.0f), m_phi(glm::pi<float>() * 0.5f),
    m_lookSpeed(lookSpeed),
    m_moveSpeed(moveSpeed),
    m_jumpSpeed(jumpSpeed),
    m_sprintSpeed(sprintSpeed),
    m_jumping(false),
    m_enabled(true),
    m_stepDistance(0.0f)
{}

void PlayerControllerComponent::init() {
    const auto & spatials(gameObject().getComponentsByType<SpatialComponent>());
    assert(spatials.size() >= 2);
    m_bodySpatial = spatials[0];
    m_headSpatial = spatials[1];
    if (!(m_newtonian = gameObject().getComponentByType<NewtonianComponent>())) assert(false);
    if (!(m_ground = gameObject().getComponentByType<GroundComponent>())) assert(false);
}

void PlayerControllerComponent::update(float dt) {
    if (!m_enabled) {
        return;
    }

    // rotate camera
    if (Mouse::dx || Mouse::dy) {
        m_theta -= float(Mouse::dx) * m_lookSpeed;
        m_phi += float(Mouse::dy) * m_lookSpeed;
        updateSpatialOrientation();
    }

    int forward(Keyboard::isKeyPressed(GLFW_KEY_W));
    int backward(Keyboard::isKeyPressed(GLFW_KEY_S));
    int left(Keyboard::isKeyPressed(GLFW_KEY_A));
    int right(Keyboard::isKeyPressed(GLFW_KEY_D));
    bool sprint(Keyboard::isKeyPressed(GLFW_KEY_LEFT_SHIFT));

    float groundSpeed(sprint ? m_sprintSpeed : m_moveSpeed);
    
    // move player
    glm::vec2 xzDir(
        float(right - left),
        float(backward - forward)
    );
    if (xzDir != glm::vec2()) {
        xzDir = Util::safeNorm(xzDir);
        // would-be direction of movement on the xz plane
        glm::vec3 dir = xzDir.x * m_bodySpatial->u() + xzDir.y * m_bodySpatial->w();
        // take into account slope of ground
        // TODO: if for whatever reason we want the player to be able to move
        // on the ground when the ground is >= 90 degrees off the xz plane, this
        // will not work
        if (m_ground->onGround()) {
            dir = Util::safeNorm(Util::projectOnto(dir, m_ground->groundNorm()));
        }
        m_bodySpatial->move(dir * groundSpeed * dt);
        // remove some velocity against direction of movement
        m_newtonian->removeSomeVelocityAgainstDir(dir, groundSpeed);

        // step distance
        m_stepDistance += (glm::length(dir) * groundSpeed * dt);

    }

    // jump
    if (!m_jumping) {
        if (Keyboard::isKeyPressed(GLFW_KEY_SPACE) && m_ground->onGround()) {
            gameObject().getComponentByType<NewtonianComponent>()->addVelocity(-m_jumpSpeed * Util::safeNorm(SpatialSystem::gravity()));
            m_jumping = true;
        

        }
        
    }
    else {
        if (!Keyboard::isKeyPressed(GLFW_KEY_SPACE)) {
            m_jumping = false;
        }
    }

    if (m_ground->onGround()) {
        // Play step sound effect only when not jumping
        if (m_stepDistance > 3.0f) {
            SoundSystem::playSound3D("footstep.wav", m_bodySpatial->position() - glm::vec3(0.0, 1.0f, 0.0f));
            m_stepDistance = 0.0f;
        }
    } 

}

void PlayerControllerComponent::setOrientation(float theta, float phi) {
    m_theta = theta;
    m_phi = phi;
    updateSpatialOrientation();
}

void PlayerControllerComponent::setEnabled(bool enabled) {
    m_enabled = enabled;
}

void PlayerControllerComponent::updateSpatialOrientation() {    
    if      (m_theta >  glm::pi<float>()) m_theta = std::fmod(m_theta, glm::pi<float>()) - glm::pi<float>();
    else if (m_theta < -glm::pi<float>()) m_theta = glm::pi<float>() - std::fmod(-m_theta, glm::pi<float>());
    if (m_phi < 0.0f) m_phi = 0.0f;
    if (m_phi > glm::pi<float>()) m_phi = glm::pi<float>();
    
    glm::vec3 bodyU(std::cos(m_theta), 0.0f, -std::sin(m_theta));
    glm::vec3 bodyW(-bodyU.z, 0.0f, bodyU.x);
    glm::vec3 headW(0.0f, -std::cos(m_phi), std::sin(m_phi));
    glm::vec3 headV(0.0f, headW.z, -headW.y);

    m_bodySpatial->setRelativeUVW(bodyU, glm::vec3(0.0f, 1.0f, 0.0f), bodyW);
    m_headSpatial->setRelativeUVW(glm::vec3(1.0f, 0.0f, 0.0f), headV, headW);
}