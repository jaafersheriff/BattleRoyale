/* System interface */
#pragma once
#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_



#include "Util/Memory.hpp"



enum class SystemID {
    gameLogic,
    pathfinding,
    enemy,
    spatial,
    collision,
    postCollision,
    render
};



// Systems are static classes, and static classes can't be polymorphic, but every
// system should resemble the following...
/*
class System {

    friend Scene;

    public:

    // setup system
    static void init();

    // update system
    static void update(float dt) = 0;

};
*/



#endif