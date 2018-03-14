#pragma once
#ifndef _GAME_LOGIC_SYSTEM_HPP_
#define _GAME_LOGIC_SYSTEM_HPP_



#include "System.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/CameraComponents/CameraControllerComponent.hpp"
#include "Component/PlayerComponents/PlayerControllerComponent.hpp"
#include "Component/ImGuiComponents/ImGuiComponent.hpp"
#include "Component/EnemyComponents/EnemyComponent.hpp"
#include "Component/WeaponComponents/ProjectileComponents.hpp"
#include "Component/WeaponComponents/BlastComponent.hpp"



// static class
class GameLogicSystem {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::gameLogic;

    public:

    static void init() {};

    static void update(float dt);

    private:

    static const Vector<CameraComponent *> & s_cameraComponents;
    static const Vector<CameraControllerComponent *> & s_cameraControllerComponents;
    static const Vector<PlayerControllerComponent *> & s_playerControllers;
    static const Vector<ImGuiComponent *> & s_imguiComponents;
    static const Vector<EnemyComponent *> & s_enemyComponents;
    static const Vector<ProjectileComponent *> & s_projectileComponents;
    static const Vector<BlastComponent *> & s_blastComponents;
};

#endif