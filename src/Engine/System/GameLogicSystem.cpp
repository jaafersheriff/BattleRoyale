#include "GameLogicSystem.hpp"



std::vector<std::unique_ptr<CameraComponent>> GameLogicSystem::s_cameraComponents;
std::vector<std::unique_ptr<CameraControllerComponent>> GameLogicSystem::s_cameraControllerComponents;
std::vector<std::unique_ptr<PlayerControllerComponent>> GameLogicSystem::s_playerControllers;
std::vector<std::unique_ptr<ImGuiComponent>> GameLogicSystem::s_imguiComponents;

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

void GameLogicSystem::add(std::unique_ptr<Component> component) {
    if (dynamic_cast<CameraComponent *>(component.get()))
        s_cameraComponents.emplace_back(static_cast<CameraComponent *>(component.release()));
    else if (dynamic_cast<CameraControllerComponent *>(component.get()))
        s_cameraControllerComponents.emplace_back(static_cast<CameraControllerComponent *>(component.release()));
    else if (dynamic_cast<PlayerControllerComponent *>(component.get()))
        s_playerControllers.emplace_back(static_cast<PlayerControllerComponent *>(component.release()));
    else if (dynamic_cast<ImGuiComponent *>(component.get()))
        s_imguiComponents.emplace_back(static_cast<ImGuiComponent *>(component.release()));
    else
        assert(false);
}

void GameLogicSystem::remove(Component * component) {
    if (dynamic_cast<CameraComponent *>(component)) {
        for (auto it(s_cameraComponents.begin()); it != s_cameraComponents.end(); ++it) {
            if (it->get() == component) {
                s_cameraComponents.erase(it);
                break;
            }
        }
    }
    else if (dynamic_cast<CameraControllerComponent *>(component)) {
        for (auto it(s_cameraControllerComponents.begin()); it != s_cameraControllerComponents.end(); ++it) {
            if (it->get() == component) {
                s_cameraControllerComponents.erase(it);
                break;
            }
        }
    }
    else if (dynamic_cast<ImGuiComponent *>(component)) {
        for (auto it(s_imguiComponents.begin()); it != s_imguiComponents.end(); ++it) {
            if (it->get() == component) {
                s_imguiComponents.erase(it);
                break;
            }
        }
    }
}