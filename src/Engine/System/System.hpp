/* System interface */
#pragma once
#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_



#include <vector>
#include <memory>



enum class SystemID {
    gameLogic,
    pathfinding,
    spatial,
    collision,
    postCollision,
    render,
    sound
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

    private:

    // scene notifying system that a component was added
    static void added(Component & component);

    // scene notifying system that a component was removed
    static void removed(Component & component);

};
*/



#endif