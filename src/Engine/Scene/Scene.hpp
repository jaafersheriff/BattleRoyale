/* Scene 
 * Contains data structures for systems, game objects, and components */
#pragma once
#ifndef _SCENE_HPP_
#define _SCENE_HPP_



#include <unordered_map>
#include <vector>
#include <memory>

#include "System/Systems.hpp"
#include "GameObject/GameObject.hpp"
#include "Component/Components.hpp"


class Scene {

    public:

        Scene();

        /* Game Objects */
        GameObject * createGameObject();
    
        // Creates a component of the given type and adds it to the scene
        template <typename T, typename... Args>
        T * createComponent(Args &&... args);

        // Adds a pre-created component to the scene
        void addComponent(std::unique_ptr<Component> component);

        /* Main update function */
        void update(float);

        /* Destroy everything */
        void shutDown();

        GameLogicSystem & gameLogicSystem() { return *m_gameLogicSystemRef; }
        RenderSystem & renderSystem() { return *m_renderSystemRef; }
        SpatialSystem & spatialSystem() { return *m_spatialSystemRef; }
        CollisionSystem & collisionSystem() { return *m_collisionSystemRef; }

    private:

        /* Instantiate/Kill queues */
        void doInitQueue();
        void doKillQueue();

    private:

        /* Systems */
        GameLogicSystem * m_gameLogicSystemRef;
        RenderSystem * m_renderSystemRef;
        SpatialSystem * m_spatialSystemRef;
        CollisionSystem * m_collisionSystemRef;

        /* Lists of all game objects */
        std::vector<GameObject *> m_gameObjectRefs;
 
        /* List of all components by system */
        std::unordered_map<System::Type, std::unique_ptr<std::vector<Component *>>> m_componentRefs;

        std::vector<std::unique_ptr<GameObject>> m_gameObjectInitQueue;
        std::vector<GameObject *> m_gameObjectKillQueue;
        std::unordered_map<System::Type, std::vector<std::unique_ptr<Component>>> m_componentInitQueue;
        std::unordered_map<System::Type, std::vector<Component *>> m_componentKillQueue;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename CompT, typename... Args>
CompT * Scene::createComponent(Args &&... args) {
    CompT * c(new CompT(args...));
    m_componentInitQueue[CompT::SystemClass::type].emplace_back(c);
    return c;
}



#endif