#include "ProjectileComponents.hpp"

#include "Scene/Scene.hpp"
#include "GameObject/Message.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Component/EnemyComponents/EnemyComponent.hpp"
#include "Component/PostCollisionComponents/GroundComponent.hpp"
#include "Component/SpatialComponents/PhysicsComponents.hpp"
#include "BlastComponent.hpp"
#include "Component/StatComponents/StatComponents.hpp"



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
    m_radius(radius),
    m_shouldDetonate(false),
    m_nBounces(0)
{}

void GrenadeComponent::init() {
    ProjectileComponent::init();

    if (!(m_ground = gameObject().getComponentByType<GroundComponent>())) assert(false);
    if (!(m_newtonian = gameObject().getComponentByType<NewtonianComponent>())) assert(false);

    auto collisionCallback([&](const Message & msg_) {
        const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));
        if (&msg.bounder1 == m_bounder) {
            // If collided with something with health, detonate
            if (msg.bounder2.gameObject().getComponentByType<HealthComponent>()) {
                m_shouldDetonate = true;
            }
        }
    });
    Scene::addReceiver<CollisionMessage>(&gameObject(), collisionCallback);

    auto bounceCallback([&] (const Message & msg_) {
        const BounceMessage & msg(static_cast<const BounceMessage &>(msg_));
        if (++m_nBounces >= 3) {
            m_shouldDetonate = true;
        }
    });
    Scene::addReceiver<BounceMessage>(&gameObject(), bounceCallback);
}

void GrenadeComponent::update(float dt) {
    if (m_shouldDetonate) {
        GameObject & blast(Scene::createGameObject());
        SpatialComponent & blastSpatial(Scene::addComponent<SpatialComponent>(blast, m_bounder->center()));
        SphereBounderComponent & blastSphere(Scene::addComponentAs<SphereBounderComponent, BounderComponent>(blast, 0, Sphere(glm::vec3(), m_radius)));
        BlastComponent & blastBlast(Scene::addComponent<BlastComponent>(blast, m_damage));

        Scene::destroyGameObject(gameObject());
    }
}
