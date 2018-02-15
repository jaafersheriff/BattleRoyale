#include "Scene.hpp"

#include <algorithm>

void Scene::init() {
    GameLogicSystem::get().init();
    SpatialSystem::get().init();
    CollisionSystem::get().init();
    RenderSystem::get().init();
}

GameObject & Scene::createGameObject() {
    m_gameObjectInitQueue.emplace_back(new GameObject());
    return *m_gameObjectInitQueue.back().get();
}

void Scene::update(float dt) {
    doInitQueue();

    /* Update systems */
    GameLogicSystem::get().update(dt);
    SpatialSystem::get().update(dt); // needs to happen before collision
    CollisionSystem::get().update(dt);
    RenderSystem::get().update(dt); // rendering should be last

    doKillQueue();
}

void Scene::doInitQueue() {
    for (auto & o : m_gameObjectInitQueue) {
        m_gameObjectsStore.emplace_back(o.release());
        m_gameObjectRefs.push_back(m_gameObjectsStore.back().get());
    }
    m_gameObjectInitQueue.clear();

    for (auto & comp : m_componentInitQueue) {
        comp->init();
        switch (comp->systemID()) {
            case SystemID::gameLogic: GameLogicSystem::get().add(std::move(comp)); break;
            case SystemID::  spatial:   SpatialSystem::get().add(std::move(comp)); break;
            case SystemID::collision: CollisionSystem::get().add(std::move(comp)); break;
            case SystemID::   render:    RenderSystem::get().add(std::move(comp)); break;
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
            if (m_gameObjectRefs[i] == *killIt) {
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
            case SystemID::gameLogic: GameLogicSystem::get().remove(comp); continue;
            case SystemID::  spatial:   SpatialSystem::get().remove(comp); continue;
            case SystemID::collision: CollisionSystem::get().remove(comp); continue;
            case SystemID::   render:    RenderSystem::get().remove(comp); continue;
        }
        // look in initialization queue
        for (auto initIt(m_componentInitQueue.begin()); initIt != m_componentInitQueue.end(); ++initIt) {
            if (initIt->get() == comp) {
                m_componentInitQueue.erase(initIt);
                break;
            }
        }
    }
    m_componentKillQueue.clear();
}