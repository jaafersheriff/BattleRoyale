#include "EnemyComponent.hpp"

#include "Scene/Scene.hpp"



EnemyComponent::EnemyComponent(GameObject & gameObject, float hp) :
    Component(gameObject),
    m_hp(hp)
{}

void EnemyComponent::update(float dt) {
    if (Util::isLE(m_hp, 0.0f)) {
        Scene::destroyGameObject(this->gameObject());
    }
}

void EnemyComponent::damage(float hp) {
    m_hp -= hp;
}

void EnemyComponent::heal(float hp) {
    m_hp += hp;
}

void EnemyComponent::setHP(float hp) {
    m_hp = hp;
}



BasicEnemyComponent::BasicEnemyComponent(GameObject & gameObject, float hp) :
    EnemyComponent(gameObject, hp)
{}
