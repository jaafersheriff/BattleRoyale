#include "ProjectileComponents.hpp"

#include "Scene/Scene.hpp"
#include "GameObject/Message.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/EnemyComponents/EnemyComponent.hpp"
#include "Component/PostCollisionComponents/GroundComponent.hpp"
#include "Component/SpatialComponents/PhysicsComponents.hpp"
#include "BlastComponent.hpp"
#include "Component/StatComponents/StatComponents.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "System/SoundSystem.hpp"



ProjectileComponent::ProjectileComponent(GameObject & gameObject, const GameObject * host) :
    Component(gameObject),
    m_host(host),
    m_bounder(nullptr)
{}

void ProjectileComponent::init() {
    if (!(m_bounder = gameObject().getComponentByType<BounderComponent>())) assert(false);
    if (!(m_newtonian = gameObject().getComponentByType<NewtonianComponent>())) assert(false);
}



BulletComponent::BulletComponent(GameObject & gameObject, const GameObject * host, float damage) :
    ProjectileComponent(gameObject, host),
    m_damage(damage)
{}

void BulletComponent::init() {
    ProjectileComponent::init();

    auto collisionCallback([&](const Message & msg_) {
        const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));
        if (&msg.bounder1 == m_bounder) {
            // If collided with something with health that's not the host, detonate
            HealthComponent * health;
            bool destroy(false);
            if ((health = msg.bounder2.gameObject().getComponentByType<HealthComponent>()) && &msg.bounder2.gameObject() != m_host && !m_alreadyCollided) {
                health->changeValue(-m_damage);
                destroy = true;
                m_alreadyCollided = true;
            }
            // If collided with static geometry, destroy bullet
            if (msg.bounder2.weight() == UINT_MAX) {
                destroy = true;
            }
            if (destroy) {
                Scene::destroyGameObject(gameObject());
                SoundSystem::playSound3D("burp2.wav", gameObject().getSpatial()->position());
            }
        }
    });
    Scene::addReceiver<CollisionMessage>(&gameObject(), collisionCallback);
}

void BulletComponent::update(float dt) {
    m_alreadyCollided = false;
}



const int GrenadeComponent::k_maxBounces = 2;
const float GrenadeComponent::k_maxFuseTime = 3.0f;

GrenadeComponent::GrenadeComponent(GameObject & gameObject, const GameObject * host, float damage, float radius) :
    ProjectileComponent(gameObject, host),
    m_damage(damage),
    m_radius(radius),
    m_shouldDetonate(false),
    m_nBounces(0),
    m_fuseTime(0.0f)
{}

void GrenadeComponent::init() {
    ProjectileComponent::init();

    if (!(m_ground = gameObject().getComponentByType<GroundComponent>())) assert(false);

    auto collisionCallback([&](const Message & msg_) {
        const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));
        if (&msg.bounder1 == m_bounder) {
            // If collided with something with health that's not the host, detonate
            if (msg.bounder2.gameObject().getComponentByType<HealthComponent>() && &msg.bounder2.gameObject() != m_host) {
                m_shouldDetonate = true;
            }
        }
    });
    Scene::addReceiver<CollisionMessage>(&gameObject(), collisionCallback);

    auto bounceCallback([&] (const Message & msg_) {
        const BounceMessage & msg(static_cast<const BounceMessage &>(msg_));
        if (++m_nBounces > k_maxBounces) {
            m_shouldDetonate = true;
        }
    });
    Scene::addReceiver<BounceMessage>(&gameObject(), bounceCallback);
}

void GrenadeComponent::update(float dt) {
    m_fuseTime += dt;
    if (m_fuseTime > k_maxFuseTime) {
        m_shouldDetonate = true;
    }

    if (m_shouldDetonate) {
        GameObject & blast(Scene::createGameObject());
        SpatialComponent & blastSpatial(Scene::addComponent<SpatialComponent>(blast, m_bounder->center()));
        SphereBounderComponent & blastSphere(Scene::addComponentAs<SphereBounderComponent, BounderComponent>(blast, 0, Sphere(glm::vec3(), m_radius)));
        BlastComponent & blastBlast(Scene::addComponent<BlastComponent>(blast, m_damage));

        SoundSystem::playSound3D("splash4.wav", gameObject().getSpatial()->position());

        Scene::destroyGameObject(gameObject());
    }
}