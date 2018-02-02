/* Scene 
 * Contains data structures for systems, game objects, and components */
#pragma once
#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "GameObject/GameObject.hpp"
#include "Component/Component.hpp"

#include <vector>

class Scene {
    public:
        Scene();

        /* Game Objects */
        GameObject* createGameObject();
        void addGameObject(GameObject *);
    
        /* Components */
        template<class T, class... Args>
        T* createComponent(Args &&...);

        /* Main udate function */
        void update(float);

        /* Destroy everything */
        void shutDown();

    private:
        /* Lists of all game objects/components */
        std::vector<GameObject *> allGameObjects;
        std::vector<Component *> allComponents;

        /* Instantiate/Kill queues */
        void addNewObjects();
        void terminateObjects();
        std::vector<GameObject *> newGameObjectQueue;
        std::vector<Component *> newComponentQueue;
};

#endif