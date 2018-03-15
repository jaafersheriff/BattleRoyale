#include "ProjectileComponent.hpp"

#include "Scene/Scene.hpp"
#include "GameObject/Message.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/EnemyComponents/EnemyComponent.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"



ProjectileComponent::ProjectileComponent(GameObject & gameObject) :
    Component(gameObject),
    m_bounder(nullptr)
{}

void ProjectileComponent::init() {
    auto collisionCallback([&](const Message & msg_) {
        EnemyComponent *ec;

        if (!(m_bounder = gameObject().getComponentByType<BounderComponent>())) assert(false);

        const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));
        if (&msg.bounder1 == m_bounder) {
            ec = msg.bounder2.gameObject().getComponentByType<EnemyComponent>();
            // If this projectile collided with an EnemyComponent...
            if (ec) {
                // Then destroy this projectile
                Scene::destroyGameObject(gameObject());
                // And modify the object with the EnemyComponent
                msg.bounder2.gameObject().getSpatial()->scaleBy(glm::vec3(1.25f));
                ec->hp = ec->hp - 25.f;
            }
        }
    });
    Scene::addReceiver<CollisionMessage>(&gameObject(), collisionCallback);
}