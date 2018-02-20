#include "Scene.hpp"

#include <algorithm>

#include "GLFW/glfw3.h"

#include "IO/Keyboard.hpp"
#include "System/GameLogicSystem.hpp"
#include "System/SpatialSystem.hpp"
#include "System/PathfindingSystem.hpp"
#include "System/CollisionSystem.hpp"
#include "System/PostCollisionSystem.hpp"
#include "System/RenderSystem.hpp"



void Scene::init() {
    GameLogicSystem::get().init();
    PathfindingSystem::get().init();
    SpatialSystem::get().init();
    CollisionSystem::get().init();
    PostCollisionSystem::get().init();
    RenderSystem::get().init();
}

GameObject & Scene::createGameObject() {
    m_gameObjectInitQueue.emplace_back(new GameObject());
    return *m_gameObjectInitQueue.back().get();
}

void Scene::update(float dt) {
    doInitQueue();

    /* Update systems */
    relayMessages();
    GameLogicSystem::get().update(dt);
    relayMessages();
    PathfindingSystem::get().update(dt);
    relayMessages();
    SpatialSystem::get().update(dt); // needs to happen before collision
    relayMessages();
    CollisionSystem::get().update(dt);
    relayMessages();
    PostCollisionSystem::get().update(dt);
    relayMessages();
    RenderSystem::get().update(dt); // rendering should be last
    relayMessages();

    doKillQueue();
}

void Scene::doInitQueue() {
    for (auto & o : m_gameObjectInitQueue) {
        m_gameObjectsStore.emplace_back(o.release());
        m_gameObjectRefs.push_back(m_gameObjectsStore.back().get());
    }
    m_gameObjectInitQueue.clear();
    
    // add components to objects
    for (int i(0); i < m_componentInitQueue.size(); ++i) {
        auto & initE(m_componentInitQueue[i]);
        GameObject * go(std::get<0>(initE));
        std::type_index typeI(std::get<1>(initE));
        auto & comp(std::get<2>(initE));
        go->addComponent(*comp.get(), typeI);
    }
    // initialize components and add to systems
    for (int i(0); i < m_componentInitQueue.size(); ++i) {
        auto & initE(m_componentInitQueue[i]);
        GameObject * go(std::get<0>(initE));
        std::type_index typeI(std::get<1>(initE));
        auto & comp(std::get<2>(initE));
        comp->init(*go);
        switch (comp->systemID()) {
            case SystemID::    gameLogic:     GameLogicSystem::get().add(std::move(comp)); break;
            case SystemID::  pathfinding:   PathfindingSystem::get().add(std::move(comp)); break;
            case SystemID::      spatial:       SpatialSystem::get().add(std::move(comp)); break;
            case SystemID::    collision:     CollisionSystem::get().add(std::move(comp)); break;
            case SystemID::postCollision: PostCollisionSystem::get().add(std::move(comp)); break;
            case SystemID::       render:        RenderSystem::get().add(std::move(comp)); break;
        }
    }
    m_componentInitQueue.clear();
}

void Scene::doKillQueue() {
    // kill game objects
    for (auto killIt(m_gameObjectKillQueue.begin()); killIt != m_gameObjectKillQueue.end(); ++killIt) {
        bool found(false);
        // look in active game objects
        for (int i(0); i < m_gameObjectRefs.size(); ++i) {
            GameObject * go(m_gameObjectRefs[i]);
            if (go == *killIt) {
                m_componentKillQueue.insert(m_componentKillQueue.end(), go->getComponents().cbegin(), go->getComponents().cend());
                m_gameObjectRefs.erase(m_gameObjectRefs.begin() + i);
                m_gameObjectsStore.erase(m_gameObjectsStore.begin() + i);
                found = true;
                break;
            }
        }
        // look in game object initialization queue
        if (!found) {
            for (auto initIt(m_gameObjectInitQueue.begin()); initIt != m_gameObjectInitQueue.end(); ++initIt) {
                if (initIt->get() == *killIt) {
                    m_gameObjectInitQueue.erase(initIt);
                    break;
                }
            }
        }
    }
    m_gameObjectKillQueue.clear();
    
    // kill components
    for (auto & comp : m_componentKillQueue) {
        // look in active components
        switch (comp->systemID()) {
            case SystemID::    gameLogic:     GameLogicSystem::get().remove(comp); continue;
            case SystemID::  pathfinding:   PathfindingSystem::get().remove(comp); continue;
            case SystemID::      spatial:       SpatialSystem::get().remove(comp); continue;
            case SystemID::    collision:     CollisionSystem::get().remove(comp); continue;
            case SystemID::postCollision: PostCollisionSystem::get().remove(comp); continue;
            case SystemID::       render:        RenderSystem::get().remove(comp); continue;
        }
        // look in initialization queue
        for (auto initIt(m_componentInitQueue.begin()); initIt != m_componentInitQueue.end(); ++initIt) {
            if (std::get<2>(*initIt).get() == comp) {
                m_componentInitQueue.erase(initIt);
                break;
            }
        }
    }
    m_componentKillQueue.clear();
}

void Scene::relayMessages() {
    for (auto & message : m_messages) {
        GameObject * gameObject(std::get<0>(message));
        std::type_index msgTypeI(std::get<1>(message));
        auto & msg(std::get<2>(message));

        // send object-level message
        if (gameObject) {
            auto receivers(gameObject->m_receivers.find(msgTypeI));
            if (receivers != gameObject->m_receivers.end()) {
                for (auto & receiver : receivers->second) {
                    receiver(*msg);
                }
            }
        }
        // send scene-level message
        auto receivers(m_receivers.find(msgTypeI));
        if (receivers != m_receivers.end()) {
            for (auto & receiver : receivers->second) {
                receiver(*msg);
            }
        }
    }
    m_messages.clear();
}