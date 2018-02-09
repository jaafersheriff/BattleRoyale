#pragma once
#ifndef _COMPONENT_HPP_
#define _COMPONENT_HPP_

#include "GameObject/GameObject.hpp"
#include "GameObject/Message.hpp"

class Component {
    public:

        Component();
        virtual void init();
        virtual void update(float);

        GameObject* getGameObject() { return gameObject; }
        void setGameObject(GameObject *go) { this->gameObject = go; }

        /* Receive a message sent by another component */
        virtual void receiveMessage(Message &);

    protected:
        /* Parent game object */
        GameObject* gameObject;
};

#endif