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

    for (CompInitE & e : m_componentInitQueue) {
        e.comp->init();
        if (e.sysI == std::type_index(typeid(GameLogicSystem))) {
            GameLogicSystem::get().add(std::move(e.comp));
        }
        else if (e.sysI == std::type_index(typeid(SpatialSystem))) {
            SpatialSystem::get().add(std::move(e.comp));
        }
        else if (e.sysI == std::type_index(typeid(CollisionSystem))) {
            CollisionSystem::get().add(std::move(e.comp));
        }
        else if (e.sysI == std::type_index(typeid(RenderSystem))) {
            RenderSystem::get().add(std::move(e.comp));
        }
    }
    m_componentInitQueue.clear();
}

void Scene::doKillQueue() {
    // kill game objects
    for (auto killIt(m_gameObjectKillQueue.begin()); killIt != m_gameObjectKillQueue.end(); ++killIt) {
        bool found(false);
        // look in active game objects        
        if (killIt->sysI == std::type_index(typeid(GameLogicSystem))) {
            GameLogicSystem::get().add(std::move(e.comp));
        }
        else if (e.sysI == std::type_index(typeid(SpatialSystem))) {
            SpatialSystem::get().add(std::move(e.comp));
        }
        else if (e.sysI == std::type_index(typeid(CollisionSystem))) {
            CollisionSystem::get().add(std::move(e.comp));
        }
        else if (e.sysI == std::type_index(typeid(RenderSystem))) {
            RenderSystem::get().add(std::move(e.comp));
        }
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
    // TODO: if killing components is happening a lot, consider caching indices
    // of these three containers by component address for O(1) performance
    for (auto & killE : m_componentKillQueue) {
        // remove from store
        for (auto it(m_componentsStore.begin()); it != m_componentsStore.end(); ++it) {
            if ((*it).get() == killE.comp) {
                m_componentsStore.erase(it);
                break;
            }
        }
        // remove from system refs
        auto & sysCompRefs(*m_compRefsBySysT[killE.sysI]);
        for (auto it(sysCompRefs.begin()); it != sysCompRefs.end(); ++it) {
            if (*it == killE.comp) {
                sysCompRefs.erase(it);
                break;
            }
        }
        // remove from component type refs
        auto & compRefs(m_compRefsByCompT[killE.compI]);
        for (auto it(compRefs.begin()); it != compRefs.end(); ++it) {
            if (*it == killE.comp) {
                compRefs.erase(it);
                break;
            }
        }
    }
    m_componentKillQueue.clear();
}

void Scene::shutDown() {
    // kill all game objects
    m_gameObjectRefs.clear();
    m_gameObjectsStore.clear();
    m_gameObjectInitQueue.clear();
    m_gameObjectKillQueue.clear();

    // kill all components
    m_componentsStore.clear();
    m_compRefsBySysT.clear();
    m_compRefsByCompT.clear();
    m_componentInitQueue.clear();
    m_componentKillQueue.clear();
}