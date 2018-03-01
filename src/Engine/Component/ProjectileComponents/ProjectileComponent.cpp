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
    auto collisionCallback([&](const Message & msg_) {
        if (!(m_bounder = gameObject().getComponentByType<BounderComponent>())) assert(false);

        const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));
        if (&msg.bounder1 == m_bounder) {
            if (msg.bounder2.gameObject().getComponentByType<EnemyComponent>()) {
                Scene::destroyGameObject(gameObject());
                msg.bounder2.gameObject().getSpatial()->scaleBy(glm::vec3(1.25f));
            }
        }
    });
    Scene::addReceiver<CollisionMessage>(&gameObject(), collisionCallback);
}