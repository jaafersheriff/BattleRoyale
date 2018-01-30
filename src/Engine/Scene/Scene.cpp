#include "Scene.hpp"

void Scene::addGameObject(GameObject *go) {
    // TODO
}

GameObject* Scene::createGameObject() {
    GameObject *go = new GameObject;
    allGameObjects.push_back(go);
    return go;
}