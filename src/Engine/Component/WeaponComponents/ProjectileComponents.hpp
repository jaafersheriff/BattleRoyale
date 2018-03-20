#pragma once



#include "glm/glm.hpp"

#include "Component/Component.hpp"



class BounderComponent;
class GroundComponent;
class NewtonianComponent;



class ProjectileComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    ProjectileComponent(GameObject & gameObject, const GameObject * host);

    public:

    ProjectileComponent(ProjectileComponent && other) = default;

    protected:

    virtual void init() override;

    protected:

    const GameObject * m_host;
    BounderComponent * m_bounder;
    NewtonianComponent * m_newtonian;

};



class BulletComponent : public ProjectileComponent {

    friend Scene;

    protected:

    BulletComponent(GameObject & gameObject, const GameObject * host, float damage);

    public:

    BulletComponent(BulletComponent && other) = default;

    virtual void init() override;

    protected:

    float m_damage;

};



class GrenadeComponent : public ProjectileComponent {

    friend Scene;

    public:

    static const int k_maxBounces;
    static const float k_maxFuseTime;

    protected:

    GrenadeComponent(GameObject & gameObject, const GameObject * host, float damage, float radius);

    virtual void init() override;

    public:

    virtual void update(float dt) override;

    protected:

    void detonate();

    protected:

    GroundComponent * m_ground;
    float m_damage;
    float m_radius;
    float m_shouldDetonate;
    int m_nBounces;
    float m_fuseTime;

};