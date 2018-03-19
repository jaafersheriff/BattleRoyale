#pragma once
#ifndef _COMPONENT_HPP_
#define _COMPONENT_HPP_

#include "GameObject/GameObject.hpp"

class Scene;

class Component {

    friend Scene;

    protected: // only scene or friends can create components

        Component(GameObject & gameObject) : m_gameObject(&gameObject) {};

    public:

        Component(const Component & other) = delete;
        Component(Component && other) = default;

        Component & operator=(const Component & other) = delete;
        Component & operator=(Component && other) = default;

        /* virtual destructor necessary for polymorphic destruction */
        virtual ~Component() = default;

    protected:

    virtual void init() {}

    public:
        
        virtual void update(float) {};

        GameObject & gameObject() { return *m_gameObject; }
        const GameObject & gameObject() const { return *m_gameObject; }

    private:

        GameObject * m_gameObject;

};

#endif