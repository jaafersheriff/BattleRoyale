#include "Scene.hpp"

#include <algorithm>

#include "Util/Depot.hpp"

Scene::Scene() :
    m_gameLogicSystemRef(nullptr),
    m_renderSystemRef(nullptr),
    m_spatialSystemRef(nullptr),
    m_collisionSystemRef(nullptr),
    m_gameObjectRefs(),
    m_componentRefs(),
    m_gameObjectInitQueue(),
    m_gameObjectKillQueue(),
    m_componentInitQueue(),
    m_componentKillQueue()
{
    /* Instantiate systems */
    m_gameLogicSystemRef = Depot<GameLogicSystem>::add(new GameLogicSystem(sysComponentRefs<GameLogicSystem>()));
    m_renderSystemRef = Depot<RenderSystem>::add(new RenderSystem(sysComponentRefs<RenderSystem>()));
    m_spatialSystemRef = Depot<SpatialSystem>::add(new SpatialSystem(sysComponentRefs<SpatialSystem>()));
    m_collisionSystemRef = Depot<CollisionSystem>::add(new CollisionSystem(sysComponentRefs<CollisionSystem>()));
}

GameObject & Scene::createGameObject() {
    m_gameObjectInitQueue.emplace_back(new GameObject());
    return *m_gameObjectInitQueue.back().get();
}

void Scene::update(float dt) {
    doInitQueue();

    /* Update systems */
    m_gameLogicSystemRef->update(dt);
    m_collisionSystemRef->update(dt);
    m_spatialSystemRef->update(dt); // needs to happen after collision
    m_renderSystemRef->update(dt); // I imagine rendering should always be last

    doKillQueue();
}

void Scene::doInitQueue() {
    for (auto & o : m_gameObjectInitQueue) {
        m_gameObjectRefs.push_back(Depot<GameObject>::add(o.release()));
    }
    m_gameObjectInitQueue.clear();

    for (auto & p : m_componentInitQueue) {
        std::vector<std::unique_ptr<Component>> & initComps(p.second);
        auto & compRefs(m_componentRefs.at(p.first));

        for (auto & comp : initComps) {
            compRefs->push_back(Depot<Component>::add(comp.release()));
            compRefs->back()->init();
        }
        
        initComps.clear();
    }
}

void Scene::doKillQueue() {
    // kill game objects
    for (auto killIt(m_gameObjectKillQueue.begin()); killIt != m_gameObjectKillQueue.end(); ++killIt) {
        bool found(false);
        // look in active game objects
        for (auto refIt(m_gameObjectRefs.begin()); refIt != m_gameObjectRefs.end(); ++refIt) {
            if (*refIt == *killIt) {
                m_gameObjectRefs.erase(refIt);
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
        Depot<GameObject>::remove(*killIt);
    }
    m_gameObjectKillQueue.clear();

    // kill components
    for (auto sysIt(m_componentKillQueue.begin()); sysIt != m_componentKillQueue.end(); ++sysIt) {
        std::type_index sysIndex(sysIt->first);
        auto & killComps(sysIt->second);

        for (auto killIt(killComps.begin()); killIt != killComps.end(); ++killIt) {
            auto & compRefs(*m_componentRefs.at(sysIndex));
            bool found(false);
            // look in active components
            for (auto refIt(compRefs.begin()); refIt != compRefs.end(); ++refIt) {
                if (*refIt == *killIt) {
                    compRefs.erase(refIt);
                    found = true;
                    break;
                }
            }
            // look in component initialization queue
            if (!found && m_componentInitQueue.count(sysIndex)) {
                auto & initRefs(m_componentInitQueue.at(sysIndex));
                for (auto initIt(initRefs.begin()); initIt != initRefs.end(); ++initIt) {
                    if (initIt->get() == *killIt) {
                        initRefs.erase(initIt);
                        break;
                    }
                }
            }
            Depot<Component>::remove(*killIt);
        }

        killComps.clear();
    }
}

void Scene::shutDown() {
    // kill all game objects
    for (auto & goRef : m_gameObjectRefs) {
        Depot<GameObject>::remove(goRef);
    }
    m_gameObjectRefs.clear();
    m_gameObjectInitQueue.clear();

    // kill all components
    for (auto & p : m_componentRefs) {
        if (p.second) {
            auto & compRefs(*p.second);
            for (auto & ref : compRefs) {
                Depot<Component>::remove(ref);
            }
            compRefs.clear();
        }
    }
    m_componentRefs.clear();
    m_componentInitQueue.clear();
}