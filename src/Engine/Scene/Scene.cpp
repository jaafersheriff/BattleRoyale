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
#include "System/SoundSystem.hpp"



Vector<UniquePtr<GameObject>> Scene::s_gameObjects;
UnorderedMap<std::type_index, UniquePtr<Vector<UniquePtr<Component>>>> Scene::s_components;

Vector<UniquePtr<GameObject>> Scene::s_gameObjectInitQueue;
Vector<GameObject *> Scene::s_gameObjectKillQueue;
Vector<std::pair<std::type_index, UniquePtr<Component>>> Scene::s_componentInitQueue;
Vector<std::pair<std::type_index, Component *>> Scene::s_componentKillQueue;

Vector<std::tuple<GameObject *, std::type_index, UniquePtr<Message>>> Scene::s_messages;
UnorderedMap<std::type_index, Vector<std::function<void (const Message &)>>> Scene::s_receivers;

float Scene::totalDT;
float Scene::initDT;
float Scene::killDT;
float Scene::gameLogicDT;
float Scene::spatialDT;
float Scene::pathfindingDT;
float Scene::collisionDT;
float Scene::postCollisionDT;
float Scene::renderDT;
float Scene::gameLogicMessagingDT;
float Scene::spatialMessagingDT;
float Scene::pathfindingMessagingDT;
float Scene::collisionMessagingDT;
float Scene::postCollisionMessagingDT;
float Scene::renderMessagingDT;

void Scene::init() {
    GameLogicSystem::init();
    PathfindingSystem::init();
    SpatialSystem::init();
    CollisionSystem::init();
    PostCollisionSystem::init();
    RenderSystem::init();
    SoundSystem::init();
}

GameObject & Scene::createGameObject() {
    s_gameObjectInitQueue.emplace_back(UniquePtr<GameObject>::make(GameObject()));
    return *s_gameObjectInitQueue.back().get();
}

void Scene::destroyGameObject(GameObject & gameObject) {
    s_gameObjectKillQueue.push_back(&gameObject);
}

void Scene::update(float dt) {
    Util::Stopwatch watch;

    doInitQueue();
    relayMessages();
    initDT = float(watch.lap());

    GameLogicSystem::update(dt);
    gameLogicDT = float(watch.lap());
    relayMessages();
    gameLogicMessagingDT = float(watch.lap());

    PathfindingSystem::update(dt);
    pathfindingDT = float(watch.lap());
    relayMessages();
    pathfindingMessagingDT = float(watch.lap());

    SpatialSystem::update(dt); // needs to happen right before collision
    spatialDT = float(watch.lap());
    relayMessages();
    spatialMessagingDT = float(watch.lap());

    CollisionSystem::update(dt);
    collisionDT = float(watch.lap());
    relayMessages();
    collisionMessagingDT = float(watch.lap());

    PostCollisionSystem::update(dt); // needs to happen after collision, go figure
    postCollisionDT = float(watch.lap());
    relayMessages();
    postCollisionMessagingDT = float(watch.lap());

    RenderSystem::update(dt); // rendering should be last
    renderDT = float(watch.lap());
    relayMessages();
    renderMessagingDT = float(watch.lap());

    // TO DO: time stuff
    SoundSystem::update(dt);
    relayMessages();

    doKillQueue();
    relayMessages();
    killDT = float(watch.lap());

    totalDT = float(watch.total());
}

void Scene::doInitQueue() {
    initGameObjects();
    initComponents();
}

void Scene::doKillQueue() {
    // remove components from game objects
    for (auto & killC : s_componentKillQueue) {
        killC.second->gameObject().removeComponent(*killC.second, killC.first);
    }

    killGameObjects();
    killComponents();
}

void Scene::initGameObjects() {
    for (auto & o : s_gameObjectInitQueue) {
        sendMessage<ObjectInitMessage>(o.get());
        s_gameObjects.emplace_back(std::move(o));
    }
    s_gameObjectInitQueue.clear();
}

void Scene::initComponents() {    
    // add components to objects
    for (int i(0); i < s_componentInitQueue.size(); ++i) {
        auto & initE(s_componentInitQueue[i]);
        auto & comp(initE.second);
        comp->gameObject().addComponent(*comp.get(), initE.first);
    }
    // add components to scene, initialize them, and indicate to systems that they've been added
    for (int i(0); i < s_componentInitQueue.size(); ++i) {
        auto & initE(s_componentInitQueue[i]);
        std::type_index typeI(initE.first);
        auto & comp(initE.second);
        auto it(s_components.find(typeI));
        if (it == s_components.end()) {
            s_components.emplace(typeI, UniquePtr<Vector<UniquePtr<Component>>>::make());
            it = s_components.find(typeI);
        }
        it->second->emplace_back(std::move(comp));
        Component & c(*it->second->back());
        c.init();
        sendMessage<ComponentAddedMessage>(&c.gameObject(), c, typeI);
    }
    s_componentInitQueue.clear();
}

void Scene::killGameObjects() {
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
}

void Scene::killComponents() {
    for (auto & killE : s_componentKillQueue) {
        std::type_index typeI(killE.first);
        Component * comp(killE.second);
        bool found(false);
        // look in active components, in reverse order
        if (s_components.count(typeI)) {
            auto & comps(*s_components.at(typeI));
            for (int i(int(comps.size()) - 1); i >= 0; --i) {
                if (comps[i].get() == comp) {
                    auto it(comps.begin() + i);
                    sendMessage<ComponentRemovedMessage>(nullptr, std::move(*it), typeI);
                    comps.erase(it);
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            // look in component initialization queue, in reverse order
            for (int i(int(s_componentInitQueue.size()) - 1); i >= 0; --i) {
                if (s_componentInitQueue[i].second.get() == comp) {
                    s_componentInitQueue.erase(s_componentInitQueue.begin() + i);
                    break;
                }
            }
        }
    }
    s_componentKillQueue.clear();
}

void Scene::relayMessages() {
    static Vector<std::tuple<GameObject *, std::type_index, UniquePtr<Message>>> s_messagesBuffer;

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