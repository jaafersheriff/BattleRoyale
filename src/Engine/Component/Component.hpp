#pragma once
#ifndef _COMPONENT_HPP_
#define _COMPONENT_HPP_

#include "GameObject/GameObject.hpp"

class Scene;

class Component {


    protected: // only scene or friends can create components

        Component() : m_gameObject(nullptr) {};

    public:

        Component(const Component & other) = delete;
        Component(Component && other) = default;

        Component & operator=(const Component & other) = delete;
        Component & operator=(Component && other) = default;

        /* virtual destructor necessary for polymorphic destruction */
        virtual ~Component() = default;

    protected:

        // assigns component to game object and initializes it
        // Init takes a game object because many components' initializations depend
        // on their game object. Additionally, this is how a component would be
        // assigned to a different game object, in which case reinitialization
        // makes sense.
        virtual void init(GameObject & go) { m_gameObject = &go; };

    public:

        virtual SystemID systemID() const = 0;
        
        virtual void update(float) {};

        GameObject * gameObject() { return m_gameObject; }
        const GameObject * gameObject() const { return m_gameObject; }

    private:

        GameObject * m_gameObject;

};

#endif