/* Scene 
 * Contains data structures for systems, game objects, and components */
#pragma once
#ifndef _SCENE_HPP_
#define _SCENE_HPP_



#include <unordered_map>
#include <vector>
#include <memory>
#include <typeinfo>
#include <typeindex>

#include "System/Systems.hpp"
#include "GameObject/GameObject.hpp"
#include "Component/Components.hpp"


class Scene {

    public:

        Scene();

        /* Game Objects */
        GameObject & createGameObject();
    
        // Creates a component of the given type and adds it to the scene
        template <typename CompT, typename... Args>
        CompT & createComponent(Args &&... args);

        // the scene takes ownership of the component
        template <typename CompT>
        CompT & addComponent(std::unique_ptr<CompT> component);

        /* Main udate function */
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

        template <typename SysType>
        std::vector<Component *> & sysComponentRefs() {
            std::type_index sysIndex(typeid(SysType));
            if (!m_componentRefs[sysIndex]) {
                m_componentRefs[sysIndex].reset(new std::vector<Component *>());
            }
            return *m_componentRefs.at(sysIndex);
        }

    private:

        /* Systems */
        GameLogicSystem * m_gameLogicSystemRef;
        RenderSystem * m_renderSystemRef;
        SpatialSystem * m_spatialSystemRef;
        CollisionSystem * m_collisionSystemRef;

        /* Lists of all game objects */
        std::vector<GameObject *> m_gameObjectRefs;
 
        /* List of all components by system */
        std::unordered_map<std::type_index, std::unique_ptr<std::vector<Component *>>> m_componentRefs;

        std::vector<std::unique_ptr<GameObject>> m_gameObjectInitQueue;
        std::vector<GameObject *> m_gameObjectKillQueue;
        std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> m_componentInitQueue;
        std::unordered_map<std::type_index, std::vector<Component *>> m_componentKillQueue;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename CompT, typename... Args>
CompT & Scene::createComponent(Args &&... args) {
    return addComponent(std::unique_ptr<CompT>(new CompT(std::forward<Args>(args)...)));
}

template <typename CompT>
CompT & Scene::addComponent(std::unique_ptr<CompT> component) {
    CompT & comp(*component);
    m_componentInitQueue[std::type_index(typeid(CompT::SystemClass))].emplace_back(std::move(component));
    return comp;
}



#endif