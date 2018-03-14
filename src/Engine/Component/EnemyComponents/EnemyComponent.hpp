#pragma once



#include "Component/Component.hpp"



class BounderComponent;



class EnemyComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    EnemyComponent(GameObject & gameObject, float hp);

    public:

    EnemyComponent(EnemyComponent && other) = default;

    protected:

    virtual void init() override {};

    public:

    virtual SystemID systemID() const override { return SystemID::gameLogic; };

    virtual void update(float dt) override;

    void damage(float hp);

    void heal(float hp);

    void setHP(float hp);

    float hp() const { return m_hp; }

    protected:

    float m_hp;

};



class BasicEnemyComponent : public EnemyComponent {

    friend Scene;

    protected:

    BasicEnemyComponent(GameObject & gameObject, float hp);

};