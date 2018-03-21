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

    virtual void update(float dt) override;

    void damage(float damage);

    protected:

    HealthComponent * m_health;
    bool m_damaged;
    float m_damageCooldown;
    float m_soundCooldown;

};