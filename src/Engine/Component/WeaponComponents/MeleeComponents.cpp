#include "MeleeComponents.hpp"

#include "Scene/Scene.hpp"
#include "GameObject/Message.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/EnemyComponents/EnemyComponent.hpp"
#include "Component/StatComponents/StatComponents.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/PlayerComponents/PlayerComponent.hpp"
#include "System/SoundSystem.hpp"



MeleeComponent::MeleeComponent(GameObject & gameObject, const SpatialComponent * hostSpatial, const glm::vec3 & offset) :
    Component(gameObject),
    m_bounder(nullptr),
    m_hostSpatial(hostSpatial),
    m_hostOffset(offset)
{}

void MeleeComponent::init() {
    assert(gameObject().getSpatial());
    assert(m_bounder = gameObject().getComponentByType<BounderComponent>());
}

void MeleeComponent::update(float dt) {
    if (m_hostSpatial) {
        gameObject().getSpatial()->move(glm::vec3(m_hostSpatial->modelMatrix() * glm::vec4(m_hostOffset, 1.0f)) - gameObject().getSpatial()->position());
        gameObject().getSpatial()->setRelativeOrientation(m_hostSpatial->orientation());
    }
}



SprayComponent::SprayComponent(GameObject & gameObject, const SpatialComponent * hostSpatial, const glm::vec3 & offset, float damage) :
    MeleeComponent(gameObject, hostSpatial, offset),
    m_damage(damage),
    m_dt(0.0f)
{}

void SprayComponent::init() {
    MeleeComponent::init();

    auto collisionCallback([&](const Message & msg_) {
        const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));
        if (&msg.bounder1 == m_bounder) {
            // Damage things that spray hits
            HealthComponent * health;
            if ((health = msg.bounder2.gameObject().getComponentByType<HealthComponent>()) && &msg.bounder2.gameObject() != &m_hostSpatial->gameObject()) {
                health->changeValue(-m_damage * m_dt);
            }
        }
    });
    Scene::addReceiver<CollisionMessage>(&gameObject(), collisionCallback);
}

void SprayComponent::update(float dt) {
    MeleeComponent::update(dt);

    m_dt = dt;
}