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



// Singleton
class Scene {

    friend EngineApp;

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

    /* Destroy everything */
    void shutDown();

    /* Game Objects */
    GameObject & createGameObject();
    
    // Creates a component of the given type and adds it to the scene
    template <typename CompT, typename... Args> CompT & createComponent(Args &&... args);

    // the scene takes ownership of the component
    template <typename CompT> CompT & addComponent(std::unique_ptr<CompT> component);

    template<typename MsgT, typename... Args> void sendMessage(Args &&... args);

    template <typename MsgT> void addReceiver(Receiver & receiver);

    const std::vector<GameObject *> & getGameObjects() const { return m_gameObjectRefs; }

  private:

    /* Instantiate/Kill queues */
    void doInitQueue();
    void doKillQueue();

  private:

    /* Lists of all game objects */
    std::vector<std::unique_ptr<GameObject>> m_gameObjectsStore;
    std::vector<GameObject *> m_gameObjectRefs;

    std::vector<std::unique_ptr<GameObject>> m_gameObjectInitQueue;
    std::vector<GameObject *> m_gameObjectKillQueue;
    std::vector<std::unique_ptr<Component>> m_componentInitQueue;
    std::vector<Component *> m_componentKillQueue;

    std::vector<std::pair<std::type_index, std::unique_ptr<Message>>> m_messages;
    std::unordered_map<std::type_index, std::vector<Receiver *>> m_receivers;

};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename CompT, typename... Args>
CompT & Scene::createComponent(Args &&... args) {
    return addComponent(std::unique_ptr<CompT>(new CompT(std::forward<Args>(args)...)));
}

template <typename CompT>
CompT & Scene::addComponent(std::unique_ptr<CompT> component) {
    CompT & comp(*component);
    m_componentInitQueue.emplace_back(std::move(component));
    return comp;
}

template<typename MsgT, typename... Args>
void Scene::sendMessage(Args &&... args) {
    m_messages.emplace_back(typeid(MsgT), new MsgT(std::forward<Args>(args...)));
}

template <typename MsgT>
void Scene::addReceiver(Receiver & receiver) {
    m_receivers[type_index(typeid(MsgT))].push_back(&receiver);
}



#endif