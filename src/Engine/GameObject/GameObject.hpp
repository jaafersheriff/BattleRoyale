/* Core game object 
 * Maintains a list of components and a system for messaging */
#pragma once
#ifndef _GAME_OBJECT_HPP_
#define _GAME_OBJECT_HPP_

#include "Component/Component.hpp"

#include <vector>

class GameObject {
    public:
        GameObject();

        void init();

        template<class T>
        T* getComponent();

        void addComponent(Component *);

        void sendMessage(Message *);

    private:
        std::vector<Component *> components;

        bool isTerminated;
};

#endif