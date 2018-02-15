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

class EngineApp;

class Scene {

    friend EngineApp;

    private:

        struct CompInitE { std::type_index sysI, compI; std::unique_ptr<Component> comp; };
        struct CompKillE { std::type_index sysI, compI; Component * comp; };

    private:

        Scene(); // only engine can create scene
        // TODO: potentially add move support
        Scene(const Scene & other) = delete; // doesn't make sense to copy scene
        Scene & operator=(const Scene & other) = delete;

    public:

        /* Game Objects */
        GameObject & createGameObject();
    
        // Creates a component of the given type and adds it to the scene
        template <typename CompT, typename... Args> CompT & createComponent(Args &&... args);

        // the scene takes ownership of the component
        template <typename CompT> CompT & addComponent(std::unique_ptr<CompT> component);

        /* Main udate function */
        void update(float);

        /* Destroy everything */
        void shutDown();

        GameLogicSystem & gameLogicSystem() { return *m_gameLogicSystem; }
        RenderSystem & renderSystem() { return *m_renderSystem; }
        SpatialSystem & spatialSystem() { return *m_spatialSystem; }
        PathfindingSystem & pathfindingSystem() { return *m_pathfindingSystem; }
        CollisionSystem & collisionSystem() { return *m_collisionSystem; }

        const std::vector<GameObject *> & getGameObjects() const { return m_gameObjectRefs; }

        template <typename SysT> const std::vector<Component *> & getComponentsBySystem() const;
        template <typename CompT> const std::vector<Component *> & getComponentsByType() const;

    private:

        /* Instantiate/Kill queues */
        void doInitQueue();
        void doKillQueue();

    private:

        /* Systems */
        std::unique_ptr<GameLogicSystem> m_gameLogicSystem;
        std::unique_ptr<RenderSystem> m_renderSystem;
        std::unique_ptr<SpatialSystem> m_spatialSystem;
        std::unique_ptr<CollisionSystem> m_collisionSystem;
        std::unique_ptr<PathfindingSystem> m_pathfindingSystem;

        /* Lists of all game objects */
        std::vector<std::unique_ptr<GameObject>> m_gameObjectsStore;
        std::vector<GameObject *> m_gameObjectRefs;
 
        // all components on the heap
        std::vector<std::unique_ptr<Component>> m_componentsStore;
        // references to components grouped by system
        std::unordered_map<std::type_index, std::unique_ptr<std::vector<Component *>>> m_compRefsBySysT;
        // references to components grouped by component type
        std::unordered_map<std::type_index, std::vector<Component *>> m_compRefsByCompT;

        std::vector<std::unique_ptr<GameObject>> m_gameObjectInitQueue;
        std::vector<GameObject *> m_gameObjectKillQueue;
        std::vector<CompInitE> m_componentInitQueue;
        std::vector<CompKillE> m_componentKillQueue;

};

// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////

template <typename CompT, typename... Args>
CompT & Scene::createComponent(Args &&... args) {
    return addComponent(std::unique_ptr<CompT>(new CompT(std::forward<Args>(args)...)));
}

template <typename CompT>
CompT & Scene::addComponent(std::unique_ptr<CompT> component) {
    CompT & comp(*component);
    m_componentInitQueue.emplace_back(CompInitE{
        std::type_index(typeid(typename CompT::SystemClass)),
        std::type_index(typeid(CompT)),
        std::move(component)
    });
    return comp;
}

template <typename SysT>
const std::vector<Component *> & Scene::getComponentsBySystem() const {
    static std::vector<Component *> s_emptyList;
    
    auto it(m_compRefsBySysT.find(std::type_index(typeid(SysT))));
    if (it != m_compRefsBySysT.end() && it->second) {
        return *it->second;
    }
    return s_emptyList;
}

template <typename CompT>
const std::vector<Component *> & Scene::getComponentsByType() const {
    static std::vector<Component *> s_emptyList;
    
    auto it(m_compRefsByCompT.find(std::type_index(typeid(CompT))));
    if (it != m_compRefsByCompT.end()) {
        return it->second;
    }
    return s_emptyList;
}

#endif