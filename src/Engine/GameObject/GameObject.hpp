/* Core game object 
 * Maintains a list of components and a system for messaging */
#pragma once
#ifndef _GAME_OBJECT_HPP_
#define _GAME_OBJECT_HPP_

#include <vector>

#include "Message.hpp"

class Component;
class SpatialComponent;

class GameObject {

    public:

        GameObject();

        void init();

        template <typename T>
        T * getComponent();

        template <typename T>
        std::vector<T *> getComponents();

        template <typename T>
        int componentCount() const;

        void addComponent(Component &);

        void sendMessage(Message &);

        int numComponents() const { return int(components.size()); }

        SpatialComponent * getSpatial() const { return spatialComponent; }

    private:

        std::vector<Component *> components;
        SpatialComponent * spatialComponent;
};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename T>
T * GameObject::getComponent() {
    T * comp;
    for (auto c : components) {
        if (dynamic_cast<T *>(c)) {
            return static_cast<T *>(comp);
        }
    }
    return nullptr;
}

template <typename T>
std::vector<T *> GameObject::getComponents() {
    std::vector<T *> comps;
    for (auto c : components) {
        if (dynamic_cast<T *>(c)) {
            comps.push_back(static_cast<T *>(c));
        }
    }
    return comps;
}

template <typename T>
int GameObject::componentCount() const {
    int n(0);
    for (auto c : components) {
        if (dynamic_cast<T *>(c)) {
            ++n;
        }
    }
    return n;
}



#endif