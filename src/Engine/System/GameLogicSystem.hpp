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
    
    static void added(Component & component) {};

    static void removed(Component & component) {};

    private:

    static const std::vector<CameraComponent *> & s_cameraComponents;
    static const std::vector<CameraControllerComponent *> & s_cameraControllerComponents;
    static const std::vector<PlayerControllerComponent *> & s_playerControllers;
    static const std::vector<ImGuiComponent *> & s_imguiComponents;

};

#endif