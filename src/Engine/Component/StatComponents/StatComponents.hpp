#pragma once



#include "Component/Component.hpp"



template <typename T>
class StatComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    StatComponent(GameObject & gameObject, T minValue, T maxValue, T value) :
        Component(gameObject),
        m_minValue(minValue),
        m_maxValue(maxValue),
        m_value(value)
    {}

    public:

    StatComponent(StatComponent && other) = default;

    public:

    void setValue(T value) {
        m_value = glm::clamp(value, m_minValue, m_maxValue);
    }

    void changeValue(T delta) {
        m_value = glm::clamp(m_value + delta, m_minValue, m_maxValue);
    }
    
    float percentage() const {
        return float(m_value - m_minValue) / float(m_maxValue - m_minValue);
    }

    T minValue() const { return m_minValue; }
    T maxValue() const { return m_maxValue; }
    T value() const { return m_value; }

    protected:

    T m_minValue;
    T m_maxValue;
    T m_value;

};



class HealthComponent : public StatComponent<float> {

    friend Scene;

    protected:

    HealthComponent(GameObject & gameObject, float maxHP) :
        StatComponent<float>(gameObject, 0.0f, maxHP, maxHP)
    {}

};



class AmmoComponent : public StatComponent<float> {

    friend Scene;

    protected:

    AmmoComponent(GameObject & gameObject, float maxAmmo) :
        StatComponent<float>(gameObject, 0.0f, maxAmmo, maxAmmo)
    {}

};