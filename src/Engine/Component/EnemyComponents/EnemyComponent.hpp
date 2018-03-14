#pragma once



#include "Component/Component.hpp"



class BounderComponent;
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

    virtual SystemID systemID() const override { return SystemID::gameLogic; };

    virtual void update(float dt) override;

    protected:

    HealthComponent * m_health;

};



class BasicEnemyComponent : public EnemyComponent {

    friend Scene;

    protected:

    BasicEnemyComponent(GameObject & gameObject);

};