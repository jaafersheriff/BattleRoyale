#include "GameLogicSystem.hpp"

#include "Scene/Scene.hpp"



const Vector<CameraComponent *> & GameLogicSystem::s_cameraComponents(Scene::getComponents<CameraComponent>());
const Vector<CameraControllerComponent *> & GameLogicSystem::s_cameraControllerComponents(Scene::getComponents<CameraControllerComponent>());
const Vector<PlayerControllerComponent *> & GameLogicSystem::s_playerControllers(Scene::getComponents<PlayerControllerComponent>());
const Vector<ImGuiComponent *> & GameLogicSystem::s_imguiComponents(Scene::getComponents<ImGuiComponent>());
const Vector<EnemyComponent *> & GameLogicSystem::s_enemyComponents(Scene::getComponents<EnemyComponent>());

void GameLogicSystem::update(float dt) {
    // this is here and not in SpatialSystem because this needs to happen right
    // at the start of the game loop
    for (auto & comp : Scene::getComponents<SpatialComponent>()) {
        comp->update(dt);
    }
    for (auto & comp : s_imguiComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_playerControllers) {
        comp->update(dt);
    }
    for (auto & comp : s_cameraControllerComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_cameraComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_enemyComponents) {
        comp->update(dt);
    }
}