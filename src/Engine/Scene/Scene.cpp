#include "Scene.hpp"
#include <algorithm>

Scene::Scene() {
    allGameObjects.clear();
    allComponents.clear();
    newGameObjectQueue.clear();
    newComponentQueue.clear();
}

void Scene::addGameObject(GameObject *go) {
    allGameObjects.push_back(go);
    // TODO : should these go in main GO list or new GO queue?
    // TODO : add to systems?
}

GameObject* Scene::createGameObject() {
    GameObject *go = new GameObject;
    newGameObjectQueue.push_back(go);
    return go;
}

void Scene::addComponent(SystemType st, Component *cp) {
    newComponentQueue.push_back(cp);
    switch (st) {
        case GAMELOGIC:
            gameLogic.addComponent(cp);
            break;
        default:
            break;
    }
}

void Scene::update(float dt) {
    addNewObjects();

    /* Update systems */
    gameLogic.update(dt);

    terminateObjects();
}

void Scene::addNewObjects() {
    allComponents.insert(allComponents.end(), newComponentQueue.begin(), newComponentQueue.end());
    allGameObjects.insert(allGameObjects.end(), newGameObjectQueue.begin(), newGameObjectQueue.end());
    newComponentQueue.clear();
    newGameObjectQueue.clear();
}

// TODO : test this works
// TODO : remove from system component lists?
void Scene::terminateObjects() {
    unsigned int size = allGameObjects.size();
    for (unsigned int i = 0; i < size; i++) {
        if (allGameObjects.at(i) && allGameObjects.at(i)->isTerminated) {
            auto go = allGameObjects.erase(allGameObjects.begin() + i);
            delete go._Ptr;
            i--;
            size--;
        }
    }
    size = allComponents.size();
    for (unsigned int i = 0; i < size; i++) {
        if (allComponents.at(i) && allComponents.at(i)->isTerminated) {
            auto cp = allComponents.erase(allComponents.begin() + i);
            delete cp._Ptr;
            i--;
            size--;
        }
    }
}

void Scene::shutDown() {
    for (auto go : allGameObjects) {
        go->isTerminated = true;
    }
    for (auto cp : allComponents) {
        cp->isTerminated = true;
    }
    terminateObjects();
}