#pragma once



#include "Game.hpp"



namespace enemies {

    constexpr float k_defBasicSpeed = 3.0f;

    void createBasic(const glm::vec3 & position);

    void enablePathfinding();

    void disablePathfinding();

}