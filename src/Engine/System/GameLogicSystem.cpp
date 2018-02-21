#include "GameLogicSystem.hpp"

#include "Scene/Scene.hpp"



const std::vector<CameraComponent *> & GameLogicSystem::s_cameraComponents(Scene::getComponents<CameraComponent>());
const std::vector<CameraControllerComponent *> & GameLogicSystem::s_cameraControllerComponents(Scene::getComponents<CameraControllerComponent>());
const std::vector<PlayerControllerComponent *> & GameLogicSystem::s_playerControllers(Scene::getComponents<PlayerControllerComponent>());
const std::vector<ImGuiComponent *> & GameLogicSystem::s_imguiComponents(Scene::getComponents<ImGuiComponent>());

void GameLogicSystem::update(float dt) {
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
}