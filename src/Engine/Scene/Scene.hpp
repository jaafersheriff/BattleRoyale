/* Scene 
 * Contains data structures for systems, game objects, and components */
#pragma once
#ifndef _SCENE_HPP_
#define _SCENE_HPP_



#include <typeinfo>
#include <typeindex>

#include "Util/Memory.hpp"
#include "System/GameLogicSystem.hpp"
#include "System/SoundSystem.hpp"
#include "System/SpatialSystem.hpp"
#include "System/PathfindingSystem.hpp"
#include "System/CollisionSystem.hpp"
#include "System/PostCollisionSystem.hpp"
#include "System/RenderSystem.hpp"
#include "System/ParticleSystem.hpp"
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
    template <typename MsgT, typename... Args> static void sendMessage(const GameObject * gameObject, Args &&... args);

    // Adds a receiver for a message type. If gameObject is null, the receiver
    // will pick up all messages of that type. If gameObject is not null, the
    // receiver will pick up only messages sent to that object
    template <typename MsgT> static void addReceiver(const GameObject * gameObject, const std::function<void (const Message &)> & receiver);

    static const Vector<GameObject *> & getGameObjects() { return reinterpret_cast<const Vector<GameObject *> &>(s_gameObjects); }

    template <typename CompT> static const Vector<CompT *> & getComponents();

  private:

    /* Initialization / kill queues */
    static void doInitQueue();
    static void doKillQueue();

    static void initGameObjects();
    static void initComponents();
    static void killGameObjects();
    static void killComponents();

    static void relayMessages();

  private:

    static Vector<UniquePtr<GameObject>> s_gameObjects;
    static UnorderedMap<std::type_index, UniquePtr<Vector<UniquePtr<Component>>>> s_components;

    static Vector<UniquePtr<GameObject>> s_gameObjectInitQueue;
    static Vector<GameObject *> s_gameObjectKillQueue;
    static Vector<std::pair<std::type_index, UniquePtr<Component>>> s_componentInitQueue;
    static Vector<std::pair<std::type_index, Component *>> s_componentKillQueue;

    static Vector<std::tuple<const GameObject *, std::type_index, UniquePtr<Message>>> s_messages;
    static UnorderedMap<std::type_index, Vector<std::function<void (const Message &)>>> s_receivers;

  public:

    static float totalDT;
    static float initDT;
    static float killDT;
    static float gameLogicDT;
    static float gameLogicMessagingDT;
    static float spatialDT;
    static float spatialMessagingDT;
    static float pathfindingDT;
    static float pathfindingMessagingDT;
    static float collisionDT;
    static float collisionMessagingDT;
    static float postCollisionDT;
    static float postCollisionMessagingDT;
    static float renderDT;
    static float renderMessagingDT;
    static float soundDT;
    static float soundMessagingDT;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename CompT, typename... Args>
CompT & Scene::addComponent(GameObject & gameObject, Args &&... args) {
    return addComponentAs<CompT, CompT, Args...>(gameObject, std::forward<Args>(args)...);
}

template <typename CompT, typename SuperT, typename... Args>
CompT & Scene::addComponentAs(GameObject & gameObject, Args &&... args) {
    static_assert(std::is_base_of<Component, CompT>::value, "CompT must be a component type");
    static_assert(std::is_base_of<SuperT, CompT>::value, "CompT must be derived from SuperT");
    static_assert(!std::is_same<CompT, Component>::value, "CompT must be a derived component type");

    s_componentInitQueue.emplace_back(typeid(SuperT), UniquePtr<CompT>::make(CompT(gameObject, std::forward<Args>(args)...)));
    return static_cast<CompT &>(*s_componentInitQueue.back().second);
}

template <typename CompT>
void Scene::removeComponent(CompT & component) {
    static_assert(std::is_base_of<Component, CompT>::value, "CompT must be a component type");
    static_assert(!std::is_same<CompT, Component>::value, "CompT must be a derived component type");

    s_componentKillQueue.emplace_back(typeid(CompT), static_cast<Component *>(&component));
}

template<typename MsgT, typename... Args>
void Scene::sendMessage(const GameObject * gameObject, Args &&... args) {
    static_assert(std::is_base_of<Message, MsgT>::value, "MsgT must be a message type");

    s_messages.emplace_back(gameObject, typeid(MsgT), UniquePtr<Message>::makeAs<MsgT>(std::forward<Args>(args)...));
}

template <typename MsgT>
void Scene::addReceiver(const GameObject * gameObject, const std::function<void (const Message &)> & receiver) {
    static_assert(std::is_base_of<Message, MsgT>::value, "MsgT must be a message type");

    if (gameObject) {
        const_cast<GameObject *>(gameObject)->m_receivers[std::type_index(typeid(MsgT))].emplace_back(receiver);
    }
    else {
        s_receivers[std::type_index(typeid(MsgT))].emplace_back(receiver);
    }
}

template <typename CompT>
const Vector<CompT *> & Scene::getComponents() {
    static_assert(std::is_base_of<Component, CompT>::value, "CompT must be a component type");
    static_assert(!std::is_same<CompT, Component>::value, "CompT must be a derived component type");

    std::type_index typeI(typeid(CompT));
    auto it(s_components.find(typeI));
    if (it == s_components.end()) {
        s_components.emplace(typeI, UniquePtr<Vector<UniquePtr<Component>>>::make());
        it = s_components.find(typeI);
    }
    // this is valid because unique_ptr<T> is exactly the same data as T *
    return reinterpret_cast<const Vector<CompT *> &>(*(it->second));
}



#endif