#include "GameLogicSystem.hpp"



std::vector<CameraComponent *> GameLogicSystem::s_cameraComponents;
std::vector<CameraControllerComponent *> GameLogicSystem::s_cameraControllerComponents;
std::vector<PlayerControllerComponent *> GameLogicSystem::s_playerControllers;
std::vector<ImGuiComponent *> GameLogicSystem::s_imguiComponents;

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

void GameLogicSystem::add(Component & component) {
    if (dynamic_cast<CameraComponent *>(&component))
        s_cameraComponents.emplace_back(static_cast<CameraComponent *>(&component));
    else if (dynamic_cast<CameraControllerComponent *>(&component))
        s_cameraControllerComponents.emplace_back(static_cast<CameraControllerComponent *>(&component));
    else if (dynamic_cast<PlayerControllerComponent *>(&component))
        s_playerControllers.emplace_back(static_cast<PlayerControllerComponent *>(&component));
    else if (dynamic_cast<ImGuiComponent *>(&component))
        s_imguiComponents.emplace_back(static_cast<ImGuiComponent *>(&component));
    else
        assert(false);
}

void GameLogicSystem::remove(Component & component) {
    if (dynamic_cast<CameraComponent *>(&component)) {
        for (auto it(s_cameraComponents.begin()); it != s_cameraComponents.end(); ++it) {
            if (*it == &component) {
                s_cameraComponents.erase(it);
                break;
            }
        }
    }
    else if (dynamic_cast<CameraControllerComponent *>(&component)) {
        for (auto it(s_cameraControllerComponents.begin()); it != s_cameraControllerComponents.end(); ++it) {
            if (*it == &component) {
                s_cameraControllerComponents.erase(it);
                break;
            }
        }
    }
    else if (dynamic_cast<ImGuiComponent *>(&component)) {
        for (auto it(s_imguiComponents.begin()); it != s_imguiComponents.end(); ++it) {
            if (*it == &component) {
                s_imguiComponents.erase(it);
                break;
            }
        }
    }
}