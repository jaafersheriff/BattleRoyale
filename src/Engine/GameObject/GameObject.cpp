#include "GameObject.hpp"

#include "Component/Component.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

GameObject::GameObject() :
    m_allComponents(),
    m_compsBySysT(),
    m_compsByCompT(),
    m_spatialComponent(nullptr)
{}

void GameObject::addComponent(Component & component, std::type_index typeI) {
    m_allComponents.push_back(&component);
    m_compsBySysT[component.systemID()].push_back(&component);
    m_compsByCompT[typeI].push_back(&component);
    if (typeI == std::type_index(typeid(SpatialComponent))) {
        m_spatialComponent = dynamic_cast<SpatialComponent *>(&component);
    }
}

const std::vector<Component *> & GameObject::getComponentsBySystem(SystemID sysID) const {
    static const std::vector<Component *> s_emptyList;

    auto it(m_compsBySysT.find(sysID));
    if (it != m_compsBySysT.end()) {
        return it->second;
    }
    return s_emptyList;
}

Component * GameObject::getComponentBySystem(SystemID sysID) {
    auto it(m_compsBySysT.find(sysID));
    if (it != m_compsBySysT.end() && it->second.size()) {
        return it->second.front();
    }
    return nullptr;
}
