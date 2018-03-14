#pragma once



#include "Component/Component.hpp"



class StatComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    StatComponent(GameObject & gameObject, float value);

    public:

    StatComponent(StatComponent && other) = default;

    public:

    virtual SystemID systemID() const override { return SystemID::gameLogic; };

    void setValue(float value);

    void changeValue(float delta);

    float value() const { return m_value; }

    protected:

    float m_value;

};



class HealthComponent : public StatComponent {

    friend Scene;

    protected:

    HealthComponent(GameObject & gameObject, float value) : StatComponent(gameObject, value) {}

};