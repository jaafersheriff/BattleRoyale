#pragma once



#include "Component/Component.hpp"



class BounderComponent;



class EnemyComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    EnemyComponent(GameObject & gameObject) : Component(gameObject) {}

    public:

    EnemyComponent(EnemyComponent && other) = default;
    float hp = 100.f;

    protected:

    virtual void init() override {};

    public:

    virtual SystemID systemID() const override { return SystemID::gameLogic; };

    virtual void update(float dt) override;
};