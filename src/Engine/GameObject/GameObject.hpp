/* Core game object 
 * Maintains a list of components and a system for messaging */
#pragma once
#ifndef _GAME_OBJECT_HPP_
#define _GAME_OBJECT_HPP_

#include "Message.hpp"
#include "Transform.hpp"

#include <vector>

class Component;
class GameObject {
    public:
        GameObject();

        void init();

        template<class T>
        T* getComponent();

        void addComponent(Component *);

        void sendMessage(Message *);

        /* Denotes if the game object should be removed */
        bool isTerminated = false;
        /* All game objects live in the world, so they all get a transformation */
        Transform transform;
    private:
        std::vector<Component *> components;
};

#endif