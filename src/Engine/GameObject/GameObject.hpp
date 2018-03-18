/* Core game object 
 * Maintains a list of components and a system for messaging */
#pragma once
#ifndef _GAME_OBJECT_HPP_
#define _GAME_OBJECT_HPP_

#include <type_traits>
#include <typeindex>
#include <functional>

#include "glm/glm.hpp"
#include "Util/Memory.hpp"

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
    template <typename CompT> const Vector<CompT *> & getComponentsByType() const;

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
    static_assert(std::is_base_of<Component, CompT>::value, "CompT must be a component type");
    static_assert(!std::is_same<CompT, Component>::value, "CompT must be a derived component type");

    addComponent(component, std::type_index(typeid(CompT)));
}

template <typename CompT>
const Vector<CompT *> & GameObject::getComponentsByType() const {
    static_assert(std::is_base_of<Component, CompT>::value, "CompT must be a component type");
    static_assert(!std::is_same<CompT, Component>::value, "CompT must be a derived component type");

    static const Vector<CompT *> s_emptyList;

    auto it(m_compsByCompT.find(std::type_index(typeid(CompT))));
    if (it != m_compsByCompT.end()) {
        return reinterpret_cast<const Vector<CompT *> &>(it->second);
    }
    return s_emptyList;
}

template <typename CompT>
CompT * GameObject::getComponentByType() const {
    static_assert(std::is_base_of<Component, CompT>::value, "CompT must be a component type");
    static_assert(!std::is_same<CompT, Component>::value, "CompT must be a derived component type");

    auto it(m_compsByCompT.find(std::type_index(typeid(CompT))));
    if (it != m_compsByCompT.end() && it->second.size()) {
        return static_cast<CompT *>(it->second.front());
    }
    return nullptr;
}



#endif
