#include "Scene.hpp"
#include <algorithm>

Scene::Scene() {
    allGameObjects.clear();
    allComponents.clear();
    createGameObjectQueue.clear();
    createComponentQueue.clear();
    killGameObjectQueue.clear();
    killComponentQueue.clear();
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

    killQueues();
}

void Scene::createQueues() {
    allComponents.insert(allComponents.end(), createComponentQueue.begin(), createComponentQueue.end());
    allGameObjects.insert(allGameObjects.end(), createGameObjectQueue.begin(), createGameObjectQueue.end());
    createComponentQueue.clear();
    createGameObjectQueue.clear();
}

void Scene::killQueues() {
    for (auto go : killGameObjectQueue) {
        allGameObjects.erase(std::remove(allGameObjects.begin(), allGameObjects.end(), go));
        delete go;
    }
    for (auto cp : killComponentQueue) {
        allComponents.erase(std::remove(allComponents.begin(), allComponents.end(), cp));
        delete cp;
    }
    killGameObjectQueue.clear();
    killComponentQueue.clear();
}