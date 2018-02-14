/* Core game object 
 * Maintains a list of components and a system for messaging */
#pragma once
#ifndef _GAME_OBJECT_HPP_
#define _GAME_OBJECT_HPP_

#include <vector>
#include <unordered_map>
#include <type_traits>
#include <typeindex>

#include "Message.hpp"

class Component;
class SpatialComponent;

class GameObject {

    public:

    GameObject();
    ~GameObject();

    void init();

    // add a component
    template <typename CompT> void addComponent(CompT & component);

    void sendMessage(Message &);

    // get all components;
    const std::vector<Component *> & getComponents() const { return m_allComponents; }
    // get all components belonging to a system
    template <typename SysT> const std::vector<Component *> & getComponentsBySystem() const;
    // get all components of a specific type
    template <typename CompT> const std::vector<Component *> & getComponentsByType() const;

    // get first component belonging to a system
    template <typename SysT> Component * getComponentBySystem();
    template <typename SysT> const Component * getComponentBySystem() const {
        return const_cast<GameObject*>(this)->getComponentBySystem<SysT>();
    }
    // get first component of a specific type
    template <typename CompT> CompT * getComponentByType();
    template <typename CompT> const CompT * getComponentByType() const {
        return const_cast<GameObject*>(this)->getComponentByType<CompT>();
    }

    // get the spatial component
    SpatialComponent * getSpatial() { return m_spatialComponent; }
    const SpatialComponent * getSpatial() const { return m_spatialComponent; }

    private:

    std::vector<Component *> m_allComponents;
    std::unordered_map<std::type_index, std::vector<Component *>> m_compsBySysT;
    std::unordered_map<std::type_index, std::vector<Component *>> m_compsByCompT;
    SpatialComponent * m_spatialComponent;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename CompT>
void GameObject::addComponent(CompT & component) {
    m_allComponents.push_back(&component);
    m_compsBySysT[std::type_index(typeid(typename CompT::SystemClass))].push_back(&component);
    m_compsByCompT[std::type_index(typeid(CompT))].push_back(&component);
    if (std::is_same<CompT, SpatialComponent>::value) {
        m_spatialComponent = dynamic_cast<SpatialComponent *>(&component);
    }
    component.setGameObject(this);
}

template <typename SysT>
const std::vector<Component *> & GameObject::getComponentsBySystem() const {
    return m_compsBySysT[std::type_index(typeid(typename SysT::SystemClass))];
}

template <typename CompT>
const std::vector<Component *> & GameObject::getComponentsByType() const {
    return m_compsByCompT[std::type_index(typeid(CompT))];
}

template <typename SysT>
Component * GameObject::getComponentBySystem() {
    auto it(m_compsBySysT.find(std::type_index(typeid(typename SysT::SystemClass))));
    if (it != m_compsBySysT.end() && it->size()) {
        return it->front();
    }
    return nullptr;
}

template <typename CompT>
CompT * GameObject::getComponentByType() {
    auto it(m_compsByCompT.find(std::type_index(typeid(CompT))));
    if (it != m_compsBySysT.end() && it->size()) {
        return static_cast<CompT *>(it->front());
    }
    return nullptr;
}



#endif