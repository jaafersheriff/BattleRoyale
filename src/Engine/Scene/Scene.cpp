#include "Scene.hpp"

#include <algorithm>

Scene::Scene() {
    /* Clear lists */
    allGameObjects.clear();
    allComponents.clear();
    newGameObjectQueue.clear();
    newComponentQueue.clear();

    /* Instantiate systems */
    gameLogic = new GameLogicSystem(&allComponents[GAMELOGIC]);
    renderer = new RenderSystem(&allComponents[RENDERABLE]);
    collision = new CollisionSystem(&allComponents[COLLISION]);
}

GameObject* Scene::createGameObject() {
    GameObject *go = new GameObject;
    newGameObjectQueue.push_back(go);
    return go;
}

void Scene::addGameObject(GameObject *go) {
    allGameObjects.push_back(go);
    // TODO : should these go in main GO list or new GO queue?
}

void Scene::addComponent(SystemType st, Component *cp) {
    newComponentQueue[st].push_back(cp);
}

void Scene::update(float dt) {
    addNewObjects();

    /* Update systems */
    gameLogic->update(dt);
    renderer->update(dt);
    collision->update(dt);

    terminateObjects();
}

void Scene::addNewObjects() {
    for (auto iter = allComponents.begin(); iter != allComponents.end(); ++iter) {
        // TODO : call component init functions
        iter->second.insert(iter->second.end(), newComponentQueue[iter->first].begin(), newComponentQueue[iter->first].end());
        newComponentQueue[iter->first].clear();
    }
    allGameObjects.insert(allGameObjects.end(), newGameObjectQueue.begin(), newGameObjectQueue.end());
    newGameObjectQueue.clear();
}

// TODO : test this works
void Scene::terminateObjects() {
    size_t size = allGameObjects.size();
    for (size_t i = 0; i < size; i++) {
        if (allGameObjects.at(i) && allGameObjects.at(i)->isTerminated) {
            auto go = allGameObjects.erase(allGameObjects.begin() + i);
            delete *go;
            i--;
            size--;
        }
    }
    for (auto iter = allComponents.begin(); iter != allComponents.end(); ++iter) {
        size = iter->second.size();
        for (size_t i = 0; i < size; i++) {
            if (!iter->second[i] || iter->second[i]->isTerminated) {
                auto cp = iter->second.erase(iter->second.begin() + i);
                delete *cp;
                i--;
                size--;
            }
        }
    }
}

void Scene::shutDown() {
    for (auto go : allGameObjects) {
        go->isTerminated = true;
    }
    for (auto cl : allComponents) {
        for (auto c : cl.second) {
            c->isTerminated = true;
        }
    }
    terminateObjects();
}