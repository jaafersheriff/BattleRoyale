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

    private:
        std::vector<GameObject *> allGameObjects;
        std::vector<Component *> allComponents;
};

#endif