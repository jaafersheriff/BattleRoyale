#include "PlayerComponent.hpp"

#include "Scene/Scene.hpp"
#include "Component/StatComponents/StatComponents.hpp"
#include "GameObject/Message.hpp"



PlayerComponent::PlayerComponent(GameObject & gameObject) :
    Component(gameObject)
{}

void PlayerComponent::init() {
    if (!(m_health = gameObject().getComponentByType<HealthComponent>())) assert(false);
}

void PlayerComponent::update(float dt) {
    if (Util::isLE(m_health->value(), 0.0f)) {
        Scene::sendMessage<PlayerDeadMessage>(&gameObject(), *this);
    }
}