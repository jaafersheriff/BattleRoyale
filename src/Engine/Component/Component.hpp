#pragma once
#ifndef _COMPONENT_HPP_
#define _COMPONENT_HPP_

#include "GameObject/Message.hpp"

class GameObject;
class Component {
    public:

        Component();
        virtual void init();
        virtual void update(float);

        GameObject* getGameObject() { return gameObject;  }
        void setGameObject(GameObject *go) { this->getGameObject = go; }

        /* Receive a message sent by another component */
        virtual void receiveMessage(Message *);

        /* Denotes if component should be removed */
        bool isTerminated;
    private:

        /* Parent game object */
        GameObject* gameObject;
};

#endif