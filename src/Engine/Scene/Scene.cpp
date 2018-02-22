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
std::unordered_map<std::type_index, std::unique_ptr<std::vector<std::unique_ptr<Component>>>> Scene::s_components;

std::vector<std::unique_ptr<GameObject>> Scene::s_gameObjectInitQueue;
std::vector<GameObject *> Scene::s_gameObjectKillQueue;
std::vector<std::tuple<GameObject *, std::type_index, std::unique_ptr<Component>>> Scene::s_componentInitQueue;
std::vector<std::pair<std::type_index, Component *>> Scene::s_componentKillQueue;

std::vector<std::tuple<GameObject *, std::type_index, UniquePtr<Message>>> Scene::s_messages;
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

void Scene::destroyGameObject(GameObject & gameObject) {
    s_gameObjectKillQueue.push_back(&gameObject);
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
    // add components to scene, initialize them, and indicate to systems that they've been added
    for (int i(0); i < s_componentInitQueue.size(); ++i) {
        auto & initE(s_componentInitQueue[i]);
        GameObject * go(std::get<0>(initE));
        std::type_index typeI(std::get<1>(initE));
        auto & comp(std::get<2>(initE));
        auto it(s_components.find(typeI));
        if (it == s_components.end()) {
            s_components[typeI].reset(new std::vector<std::unique_ptr<Component>>());
            it = s_components.find(typeI);
        }
        it->second->emplace_back(std::move(comp));
        Component & c(*it->second->back());
        switch (c.systemID()) {
            case SystemID::    gameLogic:     GameLogicSystem::added(c); break;
            case SystemID::  pathfinding:   PathfindingSystem::added(c); break;
            case SystemID::      spatial:       SpatialSystem::added(c); break;
            case SystemID::    collision:     CollisionSystem::added(c); break;
            case SystemID::postCollision: PostCollisionSystem::added(c); break;
            case SystemID::       render:        RenderSystem::added(c); break;
        }
        c.init(*go);
    }
    s_componentInitQueue.clear();
}

void Scene::doKillQueue() {
    // kill game objects
    for (auto killIt(s_gameObjectKillQueue.begin()); killIt != s_gameObjectKillQueue.end(); ++killIt) {
        bool found(false);
        // look in active game objects, in reverse order
        for (int i(int(s_gameObjects.size()) - 1); i >= 0; --i) {
            GameObject * go(s_gameObjects[i].get());
            if (go == *killIt) {
                // add game object's componets to kill queue
                for (auto compTIt(go->m_compsByCompT.begin()); compTIt != go->m_compsByCompT.end(); ++compTIt) {
                    for (auto & comp : compTIt->second) {
                        s_componentKillQueue.emplace_back(compTIt->first, comp);
                    }
                }
                s_gameObjects.erase(s_gameObjects.begin() + i);
                found = true;
                break;
            }
        }
        if (!found) {
            // look in game object initialization queue, in reverse order
            for (int i(int(s_gameObjectInitQueue.size()) - 1); i >= 0; --i) {
                if (s_gameObjectInitQueue[i].get() == *killIt) {
                    s_gameObjectInitQueue.erase(s_gameObjectInitQueue.begin() + i);
                    break;
                }
            }
        }
    }
    s_gameObjectKillQueue.clear();
    
    // kill components
    for (auto & killE : s_componentKillQueue) {
        std::type_index typeI(killE.first);
        Component * comp(killE.second);
        // remove from game object
        if (comp->gameObject()) {
            comp->gameObject()->removeComponent(*comp, typeI);
        }
        bool found(false);
        // look in active components, in reverse order
        if (s_components.count(typeI)) {
            auto & comps(*s_components.at(typeI));
            for (int i(int(comps.size()) - 1); i >= 0; --i) {
                if (comps[i].get() == comp) {
                    comps.erase(comps.begin() + i);
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            // look in component initialization queue, in reverse order
            for (int i(int(s_componentInitQueue.size()) - 1); i >= 0; --i) {
                if (std::get<2>(s_componentInitQueue[i]).get() == comp) {
                    s_componentInitQueue.erase(s_componentInitQueue.begin() + i);
                    break;
                }
            }
        }
        switch (comp->systemID()) {
            case SystemID::    gameLogic:     GameLogicSystem::removed(*comp); continue;
            case SystemID::  pathfinding:   PathfindingSystem::removed(*comp); continue;
            case SystemID::      spatial:       SpatialSystem::removed(*comp); continue;
            case SystemID::    collision:     CollisionSystem::removed(*comp); continue;
            case SystemID::postCollision: PostCollisionSystem::removed(*comp); continue;
            case SystemID::       render:        RenderSystem::removed(*comp); continue;
        }
    }
    s_componentKillQueue.clear();
}

void Scene::relayMessages() {
    static std::vector<std::tuple<GameObject *, std::type_index, UniquePtr<Message>>> s_messagesBuffer;

    while (s_messages.size()) {
        // this keeps things from breaking if messages are sent from receivers
        std::swap(s_messages, s_messagesBuffer);

        for (auto & message : s_messagesBuffer) {
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
        s_messagesBuffer.clear();
    }
}