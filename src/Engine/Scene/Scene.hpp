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

#include "GameObject/GameObject.hpp"
#include "GameObject/Message.hpp"
#include "Component/Components.hpp"



// Singleton
class Scene {

  public:

    static Scene & get() {
        static Scene s_scene;
        return s_scene;
    }

  private:

    Scene() = default;
    Scene(const Scene & other) = delete;
    Scene & operator=(const Scene & other) = delete;

  public:

    void init();

    /* Main udate function */
    void update(float);

    /* Game Objects */
    GameObject & createGameObject();
    
    // Creates a component of the given type and adds it to the game object
    template <typename CompT, typename... Args> CompT & addComponent(GameObject & gameObject, Args &&... args);

    // Sends out a message for any receivers of that message type to pick up.
    // If gameObject is not null, first sends the message locally to receivers
    // of only that object.
    template<typename MsgT, typename... Args> void sendMessage(GameObject * gameObject, Args &&... args);

    // Adds a receiver for a message type. If gameObject is null, the receiver
    // will pick up all messages of that type. If gameObject is not null, the
    // receiver will pick up only messages sent to that object
    template <typename MsgT> void addReceiver(GameObject * gameObject, const std::function<void (const Message &)> & receiver);

    const std::vector<GameObject *> & getGameObjects() const { return m_gameObjectRefs; }

  private:

    /* Instantiate/Kill queues */
    void doInitQueue();
    void doKillQueue();

    void relayMessages();

  private:

    /* Lists of all game objects */
    std::vector<std::unique_ptr<GameObject>> m_gameObjectsStore;
    std::vector<GameObject *> m_gameObjectRefs;

    std::vector<std::unique_ptr<GameObject>> m_gameObjectInitQueue;
    std::vector<GameObject *> m_gameObjectKillQueue;
    std::vector<std::tuple<GameObject *, std::type_index, std::unique_ptr<Component>>> m_componentInitQueue;
    std::vector<Component *> m_componentKillQueue;

    std::vector<std::tuple<GameObject *, std::type_index, std::unique_ptr<Message>>> m_messages;
    std::unordered_map<std::type_index, std::vector<std::function<void (const Message &)>>> m_receivers;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename CompT, typename... Args>
CompT & Scene::addComponent(GameObject & gameObject, Args &&... args) {
    CompT * comp(new CompT(std::forward<Args>(args)...));
    m_componentInitQueue.emplace_back(&gameObject, typeid(CompT), std::unique_ptr<CompT>(comp));
    return *comp;
}

template<typename MsgT, typename... Args>
void Scene::sendMessage(GameObject * gameObject, Args &&... args) {
    m_messages.emplace_back(gameObject, typeid(MsgT), new MsgT(std::forward<Args>(args)...));
}

template <typename MsgT>
void Scene::addReceiver(GameObject * gameObject, const std::function<void (const Message &)> & receiver) {
    if (gameObject) {
        gameObject->m_receivers[std::type_index(typeid(MsgT))].emplace_back(receiver);
    }
    else {
        m_receivers[std::type_index(typeid(MsgT))].emplace_back(receiver);
    }
}



#endif