#include "GameLogicSystem.hpp"



void GameLogicSystem::update(float dt) {
    for (auto & comp : m_imguiComponents) {
        comp->update(dt);
    }
    for (auto & comp : m_cameraControllers) {
        comp->update(dt);
    }
    for (auto & comp : m_cameraComponents) {
        comp->update(dt);
    }
}

void GameLogicSystem::add(std::unique_ptr<Component> component) {
    if (dynamic_cast<CameraComponent *>(component.get()))
        m_cameraComponents.emplace_back(static_cast<CameraComponent *>(component.release()));
    else if (dynamic_cast<CameraController *>(component.get()))
        m_cameraControllers.emplace_back(static_cast<CameraController *>(component.release()));
    else if (dynamic_cast<ImGuiComponent *>(component.get()))
        m_imguiComponents.emplace_back(static_cast<ImGuiComponent *>(component.release()));
}