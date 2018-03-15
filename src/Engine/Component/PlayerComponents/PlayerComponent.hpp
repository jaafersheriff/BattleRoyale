#pragma once



#include "Component/Component.hpp"



class HealthComponent;



class PlayerComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    PlayerComponent(GameObject & gameObject);

    public:

    PlayerComponent(PlayerComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual SystemID systemID() const override { return SystemID::game; };

    virtual void update(float dt) override;

    protected:

    HealthComponent * m_health;

};