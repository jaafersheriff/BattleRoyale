#include "EnemyComponent.hpp"

#include "Scene/Scene.hpp"
#include "Component/StatComponents/StatComponents.hpp"



EnemyComponent::EnemyComponent(GameObject & gameObject) :
    Component(gameObject)
{}

void EnemyComponent::init() {
    if (!(m_health = gameObject().getComponentByType<HealthComponent>())) assert(false);
}

void EnemyComponent::update(float dt) {
    if (Util::isLE(m_health->value(), 0.0f)) {
        Scene::destroyGameObject(this->gameObject());
    }
}



BasicEnemyComponent::BasicEnemyComponent(GameObject & gameObject) :
    EnemyComponent(gameObject)
{}
