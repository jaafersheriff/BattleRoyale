#include "Scene.hpp"
#include <algorithm>

Scene::Scene() {
    allGameObjects.clear();
    allComponents.clear();
    createGameObjectQueue.clear();
    createComponentQueue.clear();
}

void Scene::addGameObject(GameObject *go) {
    allGameObjects.push_back(go);
    // TODO : add to systems?
}

GameObject* Scene::createGameObject() {
    GameObject *go = new GameObject;
    allGameObjects.push_back(go);
    return go;
}

template<class T, class... Args>
T* Scene::createComponent(Args&&...args) {
    T* ptr = new T(args);
    allComponents.push_back(ptr);
    return ptr;
}

void Scene::update(float dt) {
    createQueues();

    for (auto go : allGameObjects) {
        go->update(dt);
    }

    killObjects();
}

void Scene::createQueues() {
    allComponents.insert(allComponents.end(), createComponentQueue.begin(), createComponentQueue.end());
    allGameObjects.insert(allGameObjects.end(), createGameObjectQueue.begin(), createGameObjectQueue.end());
    createComponentQueue.clear();
    createGameObjectQueue.clear();
}

void Scene::killObjects() {
    unsigned int size = allGameObjects.size();
    for (int i = 0; i < size; i++) {
        if (allGameObjects.at(i) && allGameObjects.at(i)->isDeleted) {
            auto go = allGameObjects.erase(allGameObjects.begin() + i);
            delete go._Ptr;
            i--;
            size--;
        }
    }
    unsigned int size = allComponents.size();
    for (int i = 0; i < size; i++) {
        if (allComponents.at(i) && allComponents.at(i)->isDeleted) {
            auto cp = allComponents.erase(allComponents.begin() + i);
            delete cp._Ptr;
            i--;
            size--;
        }
    }
}

void Scene::shutDown() {
    for (auto go : allGameObjects) {
        go->isDeleted = true;
    }
    for (auto cp : allComponents) {
        cp->isDeleted = true;
    }
    killObjects();
}