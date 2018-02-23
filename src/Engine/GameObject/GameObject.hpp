/* Core game object 
 * Maintains a list of components and a system for messaging */
#pragma once
#ifndef _GAME_OBJECT_HPP_
#define _GAME_OBJECT_HPP_

#include <type_traits>
#include <typeindex>
#include <functional>

#include "System/System.hpp"

class Scene;
class Component;
class SpatialComponent;
struct Message;

class GameObject {

    friend Scene;

    private: // only scene or friends can create game object

    GameObject();

    public:

    GameObject(const GameObject & other) = delete; // doesn't make sense to copy a GameObject
    GameObject(GameObject && other) = default;

    GameObject & operator=(const GameObject & other) = delete;

    public:

    private: // only scene can add components

    // add a component
    template <typename CompT> void addComponent(CompT & component);
    void addComponent(Component & component, std::type_index typeI);

    void removeComponent(Component & component, std::type_index typeI);

    public:

    // get all components;
    const Vector<Component *> & getComponents() const { return m_allComponents; }
    // get all components of a specific type
    template <typename CompT> const Vector<Component *> & getComponentsByType() const;

    // get first component of a specific type
    template <typename CompT> CompT * getComponentByType() const;

    // get the spatial component
    SpatialComponent * getSpatial() const { return m_spatialComponent; }

    private:

    Vector<Component *> m_allComponents;
    UnorderedMap<std::type_index, Vector<Component *>> m_compsByCompT;
    SpatialComponent * m_spatialComponent;
    UnorderedMap<std::type_index, Vector<std::function<void (const Message &)>>> m_receivers;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename CompT>
void GameObject::addComponent(CompT & component) {
    addComponent(component, std::type_index(typeid(CompT)));
}

template <typename CompT>
const Vector<Component *> & GameObject::getComponentsByType() const {
    static const Vector<Component *> s_emptyList;

    auto it(m_compsByCompT.find(std::type_index(typeid(CompT))));
    if (it != m_compsByCompT.end()) {
        return it->second;
    }
    return s_emptyList;
}

template <typename CompT>
CompT * GameObject::getComponentByType() const {
    auto it(m_compsByCompT.find(std::type_index(typeid(CompT))));
    if (it != m_compsByCompT.end() && it->second.size()) {
        return static_cast<CompT *>(it->second.front());
    }
    return nullptr;
}



#endif
