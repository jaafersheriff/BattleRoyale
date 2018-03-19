#include "EnemyComponent.hpp"

#include "Scene/Scene.hpp"
#include "Util/Util.hpp"
#include "Component/StatComponents/StatComponents.hpp"
#include "System/GameInterface.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"



EnemyComponent::EnemyComponent(GameObject & gameObject) :
    Component(gameObject)
{}

void EnemyComponent::init() {
    if (!(m_spatial = gameObject().getComponentByType<SpatialComponent>())) assert(false);
    if (!(m_health = gameObject().getComponentByType<HealthComponent>())) assert(false);
}

void EnemyComponent::update(float dt) {
    if (Util::isLE(m_health->value(), 0.0f)) {
        Scene::destroyGameObject(this->gameObject());
    }
    glm::vec3 playerPos(GameInterface::getPlayer().getSpatial()->position());
    m_spatial->lookAt(playerPos, glm::vec3(0.0f, 1.0f, 0.0f));
}



BasicEnemyComponent::BasicEnemyComponent(GameObject & gameObject) :
    EnemyComponent(gameObject)
{}
