#pragma once



#include "Component/Component.hpp"



class HealthComponent;



class EnemyComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    EnemyComponent(GameObject & gameObject);

    public:

    EnemyComponent(EnemyComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual void update(float dt) override;

    protected:

    SpatialComponent * m_body;
    SpatialComponent * m_head;
    HealthComponent * m_health;

};



class BasicEnemyComponent : public EnemyComponent {

    friend Scene;

    protected:

    BasicEnemyComponent(GameObject & gameObject);

};