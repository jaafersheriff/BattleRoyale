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
    // get all components of a specific type
    template <typename CompT> const std::vector<Component *> & getComponents() const;

    // get first component of a specific type
    template <typename CompT> CompT * getComponent();
    template <typename CompT> const CompT * getComponent() const {
        return const_cast<GameObject*>(this)->getComponent<CompT>();
    }

    // get the spatial component
    SpatialComponent * getSpatial() { return m_spatialComponent; }
    const SpatialComponent * getSpatial() const { return m_spatialComponent; }

    // how many total components the game object has
    int numComponents() const { return int(m_components.size()); }

    // how many of a certain type of component the game object has
    template <typename CompT> int numComponents() const;

    private:

    std::vector<Component *> m_allComponents;
    std::unordered_map<std::type_index, std::vector<Component *>> m_components;
    SpatialComponent * m_spatialComponent;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename CompT>
void GameObject::addComponent(CompT & component) {
    component.setGameObject(this);
    m_components[std::type_index(typeid(CompT::SystemClass))].push_back(&component);
    if (std::is_same_v<CompT, SpatialComponent>) {
        m_spatialComponent = dynamic_cast<SpatialComponent *>(&component);
    }
}

template <typename CompT>
const std::vector<Component *> & GameObject::getComponents() const {
    return m_components[std::type_index(typeid(CompT::SystemClass))];
}

template <typename CompT>
CompT * GameObject::getComponent() {
    std::type_index sysI(typeid(CompT::SystemClass));

    if (m_components[sysI].size()) {
        return static_cast<CompT *>(m_components[sysI].front());
    }

    return nullptr;
}

template <typename CompT>
int GameObject::numComponents() const {
    return int(m_components[std::type_index(typeid(CompT::SystemClass))].size());
}



#endif