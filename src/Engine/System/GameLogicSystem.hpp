#pragma once
#ifndef _GAME_LOGIC_SYSTEM_HPP_
#define _GAME_LOGIC_SYSTEM_HPP_

#include "System.hpp"

class GameLogicSystem : public System {

    friend Scene;

    private: // only scene can create system

    GameLogicSystem(std::vector<Component *> & components);

};

#endif