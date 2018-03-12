#pragma once



#include "Game.hpp"



namespace enemies {

    void createBasic(const glm::vec3 & position);

    void enablePathfinding();

    void disablePathfinding();

}