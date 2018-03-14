#pragma once



#include "Game.hpp"



namespace weapons {

    namespace grenade {

        void fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel);
        void firePlayer();

    }

    void destroyAllProjectiles();

}