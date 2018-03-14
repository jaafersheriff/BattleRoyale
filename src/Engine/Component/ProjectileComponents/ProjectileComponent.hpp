#pragma once



#include "glm/glm.hpp"

#include "Component/Component.hpp"



class BounderComponent;



class ProjectileComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    ProjectileComponent(GameObject & gameObject);

    public:

    ProjectileComponent(ProjectileComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual SystemID systemID() const override { return SystemID::gameLogic; };

    virtual void update(float dt) override {};

    protected:

    BounderComponent * m_bounder;

};



class GrenadeComponent : public ProjectileComponent {

    friend Scene;

    protected:

    GrenadeComponent(GameObject & gameObject, float damage, float radius);

    virtual void init() override;

    protected:

    void detonate();

    protected:

    float m_damage;
    float m_radius;

};