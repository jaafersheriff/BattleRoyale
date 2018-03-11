#pragma once



#include "Game.hpp"



namespace player {

    constexpr float k_defHeight = 1.75f;
    constexpr float k_defWidth = k_defHeight / 4.0f;
    constexpr float k_defLookSpeed = 0.005f;
    constexpr float k_defMoveSpeed = 5.0f;
    constexpr float k_defSprintSpeed = 15.0f;
    constexpr float k_defJumpSpeed = 5.0f;
    constexpr float k_defFOV = 45.0f;
    constexpr float k_defNear = 0.1f;
    constexpr float k_defFar = 300.0f;

    void setup(const glm::vec3 & position);

    const GameObject & gameObject();
    
    const SpatialComponent & spatial();

    const CameraComponent & camera();

    PlayerControllerComponent & controller();

}