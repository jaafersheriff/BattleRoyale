#include "Scene.hpp"

void Scene::addGameObject(GameObject *go) {
    // TODO
}

GameObject* Scene::createGameObject() {
    GameObject *go = new GameObject;
    allGameObjects.push_back(go);
    return go;
}

template<class T, class... Args>
T* Scene::createComponent(Args&&..args) {
    T* ptr = new T(args);
    allComponents.push_back(ptr);
    return ptr;
}