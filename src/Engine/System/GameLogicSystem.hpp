#pragma once
#ifndef _GAME_LOGIC_SYSTEM_HPP_
#define _GAME_LOGIC_SYSTEM_HPP_

#include "System.hpp"

class GameLogicSystem : public System {
    public:
        GameLogicSystem(std::vector<Component *> *cp) :
            System(cp)
        {}

        void update(float);
};

#endif