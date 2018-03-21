#include "BlastComponent.hpp"

#include "Component/CollisionComponents/BounderComponent.hpp"
#include "GameObject/Message.hpp"
#include "Scene/Scene.hpp"
#include "Component/StatComponents/StatComponents.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "System/ParticleSystem.hpp"
#include "Component/ParticleComponents/ParticleAssasinComponent.hpp"
#include "Component/EnemyComponents/EnemyComponent.hpp"
#include "Component/PlayerComponents/PlayerComponent.hpp"



BlastComponent::BlastComponent(GameObject & gameObject, float damage) :
    Component(gameObject),
    m_bounder(nullptr),
    m_damage(damage),
    m_updated(false)
{}

void BlastComponent::init() {
    if (!(m_bounder = dynamic_cast<SphereBounderComponent *>(gameObject().getComponentByType<BounderComponent>()))) assert(false);

    auto collisionCallback([&](const Message & msg_) {
        const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));
        const GameObject & go(msg.bounder2.gameObject());
        const SpatialComponent * spat(go.getSpatial());
        HealthComponent * health(go.getComponentByType<HealthComponent>());
        if (health) {
            float d2(glm::distance2(spat->position(), m_bounder->center()));
            float r(m_bounder->transSphere().radius);
            if (d2 < r * r) {
                float proximity(1.0f - std::sqrt(d2) / r);
                EnemyComponent * enemy;
                PlayerComponent * player;
                if (enemy = msg.bounder2.gameObject().getComponentByType<EnemyComponent>()) {
                    enemy->damage(m_damage * proximity);
                }
                else if (player = msg.bounder2.gameObject().getComponentByType<PlayerComponent>()){
                    player->damage(m_damage * proximity);
                }
                else {
                    health->changeValue(-m_damage * proximity);
                }
            }
        }
    });
    Scene::addReceiver<CollisionMessage>(&gameObject(), collisionCallback);
}

void BlastComponent::update(float dt) {
    if (m_updated) {
        Scene::removeComponent<BlastComponent>(*this);
        Scene::removeComponent(static_cast<BounderComponent &>(*m_bounder));
        ParticleSystem::addSodaGrenadePC(*gameObject().getSpatial());
        Scene::addComponent<ParticleAssasinComponent>(gameObject());
    }

    m_updated = true;
}