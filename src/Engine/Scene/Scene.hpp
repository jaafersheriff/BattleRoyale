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

#include "System/GameLogicSystem.hpp"
#include "System/SpatialSystem.hpp"
#include "System/PathfindingSystem.hpp"
#include "System/CollisionSystem.hpp"
#include "System/RenderSystem.hpp"
#include "GameObject/GameObject.hpp"
#include "GameObject/Message.hpp"
#include "Component/Components.hpp"



// static class
class Scene {

  public:

    static void init();

    static void update(float);

    static GameObject & createGameObject();

    static void destroyGameObject(GameObject & gameObject);
    
    // Creates a component of the given type and adds it to the game object
    template <typename CompT, typename... Args> static CompT & addComponent(GameObject & gameObject, Args &&... args);
    // Like addComponent, but registers component as SuperT
    template <typename CompT, typename SuperT, typename... Args> static CompT & addComponentAs(GameObject & gameObject, Args &&... args);
    
    // Removes the component from the scene and from its game object.
    // CompT should be the derived component type, not plain Component.
    template <typename CompT> static void removeComponent(CompT & component);

    // Sends out a message for any receivers of that message type to pick up.
    // If gameObject is not null, first sends the message locally to receivers
    // of only that object.
    template <typename MsgT, typename... Args> static void sendMessage(GameObject * gameObject, Args &&... args);

    // Adds a receiver for a message type. If gameObject is null, the receiver
    // will pick up all messages of that type. If gameObject is not null, the
    // receiver will pick up only messages sent to that object
    template <typename MsgT> static void addReceiver(GameObject * gameObject, const std::function<void (const Message &)> & receiver);

    static const std::vector<GameObject *> & getGameObjects() { return reinterpret_cast<const std::vector<GameObject *> &>(s_gameObjects); }

    template <typename CompT> static const std::vector<CompT *> & getComponents();

  private:

    /* Instantiate/Kill queues */
    static void doInitQueue();
    static void doKillQueue();

    static void relayMessages();

  private:

    static std::vector<std::unique_ptr<GameObject>> s_gameObjects;
    static std::unordered_map<std::type_index, std::unique_ptr<std::vector<std::unique_ptr<Component>>>> s_components;

    static std::vector<std::unique_ptr<GameObject>> s_gameObjectInitQueue;
    static std::vector<GameObject *> s_gameObjectKillQueue;
    static std::vector<std::tuple<GameObject *, std::type_index, std::unique_ptr<Component>>> s_componentInitQueue;
    static std::vector<std::pair<std::type_index, Component *>> s_componentKillQueue;

    static std::vector<std::tuple<GameObject *, std::type_index, std::unique_ptr<Message>>> s_messages;
    static std::unordered_map<std::type_index, std::vector<std::function<void (const Message &)>>> s_receivers;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename CompT, typename... Args>
CompT & Scene::addComponent(GameObject & gameObject, Args &&... args) {
    return addComponentAs<CompT, CompT, Args...>(gameObject, std::forward<Args>(args)...);
}

template <typename CompT, typename SuperT, typename... Args>
CompT & Scene::addComponentAs(GameObject & gameObject, Args &&... args) {
    CompT * comp(new CompT(std::forward<Args>(args)...));
    s_componentInitQueue.emplace_back(&gameObject, typeid(SuperT), std::unique_ptr<CompT>(comp));
    return *comp;
}

template <typename CompT>
void Scene::removeComponent(CompT & component) {
    static_assert(!std::is_same<CompT, Component>::value, "CompT must be the derived component type");
    s_componentKillQueue.emplace_back(typeid(CompT), &component);
}

template<typename MsgT, typename... Args>
void Scene::sendMessage(GameObject * gameObject, Args &&... args) {
    s_messages.emplace_back(gameObject, typeid(MsgT), new MsgT(std::forward<Args>(args)...));
}

template <typename MsgT>
void Scene::addReceiver(GameObject * gameObject, const std::function<void (const Message &)> & receiver) {
    if (gameObject) {
        gameObject->m_receivers[std::type_index(typeid(MsgT))].emplace_back(receiver);
    }
    else {
        s_receivers[std::type_index(typeid(MsgT))].emplace_back(receiver);
    }
}

template <typename CompT>
const std::vector<CompT *> & Scene::getComponents() {
    std::type_index typeI(typeid(CompT));
    auto it(s_components.find(typeI));
    if (it == s_components.end()) {
        s_components[typeI].reset(new std::vector<std::unique_ptr<Component>>());
        it = s_components.find(typeI);
    }
    // this is valid because unique_ptr<T> is exactly the same data as T *
    return reinterpret_cast<const std::vector<CompT *> &>(*(it->second));
}



#endif