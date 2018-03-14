#include "ProjectileComponent.hpp"

#include "Scene/Scene.hpp"
#include "GameObject/Message.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/EnemyComponents/EnemyComponent.hpp"



ProjectileComponent::ProjectileComponent(GameObject & gameObject) :
    Component(gameObject),
    m_bounder(nullptr)
{}

void ProjectileComponent::init() {
    if (!(m_bounder = gameObject().getComponentByType<BounderComponent>())) assert(false);
}



GrenadeComponent::GrenadeComponent(GameObject & gameObject, float damage, float radius) :
    ProjectileComponent(gameObject),
    m_damage(damage),
    m_radius(radius)
{}

void GrenadeComponent::init() {
    ProjectileComponent::init();    

    auto collisionCallback([&](const Message & msg_) {
        const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));
        if (&msg.bounder1 == m_bounder) {
            EnemyComponent * ec(msg.bounder2.gameObject().getComponentByType<EnemyComponent>());
            // If this projectile collided with an EnemyComponent...
            if (ec) {
                detonate();
            }
        }
    });
    Scene::addReceiver<CollisionMessage>(&gameObject(), collisionCallback);
}

void GrenadeComponent::detonate() {

    Scene::destroyGameObject(gameObject());
}
