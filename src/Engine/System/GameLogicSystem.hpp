#pragma once
#ifndef _GAME_LOGIC_SYSTEM_HPP_
#define _GAME_LOGIC_SYSTEM_HPP_



#include "System.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/CameraComponents/CameraControllerComponent.hpp"
#include "Component/PlayerComponents/PlayerControllerComponent.hpp"
#include "Component/ImGuiComponents/ImGuiComponent.hpp"



// static class
class GameLogicSystem {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::gameLogic;

    public:

    static void init() {};

    static void update(float dt);

    private:
    
    static void add(std::unique_ptr<Component> component);

    static void remove(Component * component);

    private:

    static std::vector<std::unique_ptr<CameraComponent>> s_cameraComponents;
    static std::vector<std::unique_ptr<CameraControllerComponent>> s_cameraControllerComponents;
    static std::vector<std::unique_ptr<PlayerControllerComponent>> s_playerControllers;
    static std::vector<std::unique_ptr<ImGuiComponent>> s_imguiComponents;

};

#endif