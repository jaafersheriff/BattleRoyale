#include "GameObject.hpp"

#include "Component/Component.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

GameObject::GameObject() :
    m_allComponents(),
    m_compsByCompT(),
    m_spatialComponent(nullptr)
{}

void GameObject::addComponent(Component & component, std::type_index typeI) {
    m_allComponents.push_back(&component);
    m_compsByCompT[typeI].push_back(&component);
    if (typeI == std::type_index(typeid(SpatialComponent))) {
        m_spatialComponent = dynamic_cast<SpatialComponent *>(&component);
    }
}

void GameObject::removeComponent(Component & component, std::type_index typeI) {
    // remove from allComponents
    for (auto it(m_allComponents.begin()); it != m_allComponents.end(); ++it) {
        if (*it == &component) {
            m_allComponents.erase(it);
            break;
        }
    }
    // remove from compsByCompT in reverse order
    auto compsIt(m_compsByCompT.find(typeI));
    if (compsIt != m_compsByCompT.end()) {
        auto & comps(compsIt->second);
        for (int i(int(comps.size()) - 1); i >= 0; --i) {
            if (comps[i] == &component) {
                comps.erase(comps.begin() + i);
                break;
            }
        }
    }
    if (m_spatialComponent == &component) {
        m_spatialComponent = nullptr;
    }
}
