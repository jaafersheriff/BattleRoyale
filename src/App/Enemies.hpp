#pragma once



#include "Game.hpp"



namespace enemies {

    namespace basic {

        void create(const glm::vec3 & position);

        void spawn();

    }

    void enablePathfinding();

    void disablePathfinding();

}