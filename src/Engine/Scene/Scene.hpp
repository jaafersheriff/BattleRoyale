/* Scene 
 * Contains data structures for systems, game objects, and components */
#pragma once
#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "GameObject/GameObject.hpp"

#include <vector>

class Scene {
    public:
        Scene();

        /* Game Objects */
        GameObject* createGameObject();
        void addGameObject(GameObject *);

    private:
        std::vector<GameObject *> allGameObjects;

};

#endif