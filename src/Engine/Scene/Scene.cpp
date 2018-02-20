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



std::vector<std::unique_ptr<GameObject>> Scene::s_gameObjects;
std::vector<std::unique_ptr<Component>> Scene::s_components;

std::vector<std::unique_ptr<GameObject>> Scene::s_gameObjectInitQueue;
std::vector<GameObject *> Scene::s_gameObjectKillQueue;
std::vector<std::tuple<GameObject *, std::type_index, std::unique_ptr<Component>>> Scene::s_componentInitQueue;
std::vector<Component *> Scene::s_componentKillQueue;

std::vector<std::tuple<GameObject *, std::type_index, std::unique_ptr<Message>>> Scene::s_messages;
std::unordered_map<std::type_index, std::vector<std::function<void (const Message &)>>> Scene::s_receivers;

void Scene::init() {
    GameLogicSystem::init();
    PathfindingSystem::init();
    SpatialSystem::init();
    CollisionSystem::init();
    PostCollisionSystem::init();
    RenderSystem::init();
}

GameObject & Scene::createGameObject() {
    s_gameObjectInitQueue.emplace_back(new GameObject());
    return *s_gameObjectInitQueue.back().get();
}

void Scene::update(float dt) {
    doInitQueue();

    /* Update systems */
    relayMessages();
    GameLogicSystem::update(dt);
    relayMessages();
    PathfindingSystem::update(dt);
    relayMessages();
    SpatialSystem::update(dt); // needs to happen before collision
    relayMessages();
    CollisionSystem::update(dt);
    relayMessages();
    PostCollisionSystem::update(dt);
    relayMessages();
    RenderSystem::update(dt); // rendering should be last
    relayMessages();

    doKillQueue();
}

void Scene::doInitQueue() {
    for (auto & o : s_gameObjectInitQueue) {
        s_gameObjects.emplace_back(o.release());
    }
    s_gameObjectInitQueue.clear();
    
    // add components to objects
    for (int i(0); i < s_componentInitQueue.size(); ++i) {
        auto & initE(s_componentInitQueue[i]);
        GameObject * go(std::get<0>(initE));
        std::type_index typeI(std::get<1>(initE));
        auto & comp(std::get<2>(initE));
        go->addComponent(*comp.get(), typeI);
    }
    // initialize components and add to systems
    for (int i(0); i < s_componentInitQueue.size(); ++i) {
        auto & initE(s_componentInitQueue[i]);
        GameObject * go(std::get<0>(initE));
        std::type_index typeI(std::get<1>(initE));
        auto & comp(std::get<2>(initE));
        comp->init(*go);
        switch (comp->systemID()) {
            case SystemID::    gameLogic:     GameLogicSystem::add(*comp); break;
            case SystemID::  pathfinding:   PathfindingSystem::add(*comp); break;
            case SystemID::      spatial:       SpatialSystem::add(*comp); break;
            case SystemID::    collision:     CollisionSystem::add(*comp); break;
            case SystemID::postCollision: PostCollisionSystem::add(*comp); break;
            case SystemID::       render:        RenderSystem::add(*comp); break;
        }
        s_components.emplace_back(std::move(comp));
    }
    s_componentInitQueue.clear();
}

void Scene::doKillQueue() {
    // kill game objects
    for (auto killIt(s_gameObjectKillQueue.begin()); killIt != s_gameObjectKillQueue.end(); ++killIt) {
        bool found(false);
        // look in active game objects
        for (auto it(s_gameObjects.begin()); it != s_gameObjects.end(); ++it) {
            GameObject * go(it->get());
            if (go == *killIt) {
                s_componentKillQueue.insert(s_componentKillQueue.end(), go->getComponents().cbegin(), go->getComponents().cend());
                s_gameObjects.erase(it);
                found = true;
                break;
            }
        }
        if (!found) {
            // look in game object initialization queue
            for (auto initIt(s_gameObjectInitQueue.begin()); initIt != s_gameObjectInitQueue.end(); ++initIt) {
                if (initIt->get() == *killIt) {
                    s_gameObjectInitQueue.erase(initIt);
                    break;
                }
            }
        }
    }
    s_gameObjectKillQueue.clear();
    
    // kill components
    for (auto & comp : s_componentKillQueue) {
        bool found(false);
        // look in active components
        switch (comp->systemID()) {
            case SystemID::    gameLogic:     GameLogicSystem::remove(*comp); continue;
            case SystemID::  pathfinding:   PathfindingSystem::remove(*comp); continue;
            case SystemID::      spatial:       SpatialSystem::remove(*comp); continue;
            case SystemID::    collision:     CollisionSystem::remove(*comp); continue;
            case SystemID::postCollision: PostCollisionSystem::remove(*comp); continue;
            case SystemID::       render:        RenderSystem::remove(*comp); continue;
        }
        for (auto it(s_components.begin()); it != s_components.end(); ++it) {
            if (it->get() == comp) {
                s_components.erase(it);
                found = true;
                break;
            }
        }
        if (!found) {
            // look in component initialization queue
            for (auto initIt(s_componentInitQueue.begin()); initIt != s_componentInitQueue.end(); ++initIt) {
                if (std::get<2>(*initIt).get() == comp) {
                    s_componentInitQueue.erase(initIt);
                    break;
                }
            }
        }
    }
    s_componentKillQueue.clear();
}

void Scene::relayMessages() {
    for (auto & message : s_messages) {
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
        auto receivers(s_receivers.find(msgTypeI));
        if (receivers != s_receivers.end()) {
            for (auto & receiver : receivers->second) {
                receiver(*msg);
            }
        }
    }
    s_messages.clear();
}