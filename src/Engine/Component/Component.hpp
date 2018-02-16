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

    public:

        /* virtual destructor necessary for polymorphic destruction */
        virtual ~Component() = default;

        virtual SystemID systemID() const = 0;

        virtual void init() {};
        
        virtual void update(float) {};

        GameObject * getGameObject() { return m_gameObject; }
        const GameObject * getGameObject() const { return m_gameObject; }

        void setGameObject(GameObject * go) { m_gameObject = go; }

        /* Receive a message sent by another component */
        //virtual void receiveMessage(Message &);



    protected:
        /* Parent game object */
        GameObject * m_gameObject;
};

#endif