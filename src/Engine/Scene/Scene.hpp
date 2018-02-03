/* Scene 
 * Contains data structures for systems, game objects, and components */
#pragma once
#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "System/Systems.hpp"

#include "GameObject/GameObject.hpp"
#include "Component/Components.hpp"

#include <vector>

class Scene {
    public:
        enum SystemType{
            GAMELOGIC
        };
        Scene();

        /* Game Objects */
        GameObject* createGameObject();
        void addGameObject(GameObject *);
    
        /* Components */
        void addComponent(SystemType, Component *);

        /* Main udate function */
        void update(float);

        /* Destroy everything */
        void shutDown();
    private:
        GameLogicSystem gameLogic;

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