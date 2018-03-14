#pragma once



#include "Game.hpp"



namespace enemies {

    namespace basic {

        void create(const glm::vec3 & position);

    }

    void enablePathfinding();

    void disablePathfinding();

}