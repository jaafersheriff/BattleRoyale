#include "PlayerComponent.hpp"

#include "Scene/Scene.hpp"
#include "Component/StatComponents/StatComponents.hpp"
#include "GameObject/Message.hpp"
#include "Util/Util.hpp"
#include "System/SoundSystem.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"



PlayerComponent::PlayerComponent(GameObject & gameObject) :
    Component(gameObject),
    m_health(nullptr),
    m_damaged(false),
    m_soundCooldown(0.0f)
{}

void PlayerComponent::init() {
    if (!(m_health = gameObject().getComponentByType<HealthComponent>())) assert(false);
}

void PlayerComponent::update(float dt) {
    m_soundCooldown -= dt;
    if (m_health->value() < 0.5f) {
        Scene::sendMessage<PlayerDeadMessage>(&gameObject(), *this);
        return;
    }
    else if (m_damaged && m_soundCooldown <= 0.0f) {
        SoundSystem::playSound3D("grunt3.wav", gameObject().getSpatial()->position());
        m_soundCooldown = 0.25f;
    }
    m_damaged = false;

    // passive heal, 10% per second
    m_health->changeValue(m_health->maxValue() * 0.1f * dt);
}

void PlayerComponent::damage(float damage) {    
    m_health->changeValue(-damage);
    m_damaged = true;
}