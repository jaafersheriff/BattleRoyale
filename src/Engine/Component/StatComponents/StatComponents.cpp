#include "StatComponents.hpp"



StatComponent::StatComponent(GameObject & gameObject, float value) :
    Component(gameObject),
    m_value(value)
{}

void StatComponent::setValue(float value) {
    m_value = value;
}

void StatComponent::changeValue(float delta) {
    m_value += delta;
}