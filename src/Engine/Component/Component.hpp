#pragma once
#ifndef _COMPONENT_HPP_
#define _COMPONENT_HPP_

#include "GameObject/GameObject.hpp"

class Scene;

class Component {

        friend Scene;

    protected: // only scene or friends can create components

        Component() :
            m_gameObject(nullptr)
        {};

        // TODO: potentially add move support
        Component(const Component & other) = default;
        Component & operator=(const Component & other) = default;

        // assigns component to game object and initializes it
        virtual void init(GameObject & gameObject) { m_gameObject = &gameObject; };

    public:

        /* virtual destructor necessary for polymorphic destruction */
        virtual ~Component() = default;

        virtual SystemID systemID() const = 0;
        
        virtual void update(float) {};

        GameObject * getGameObject() { return m_gameObject; }
        const GameObject * getGameObject() const { return m_gameObject; }

    protected:

        GameObject * m_gameObject;

};

#endif