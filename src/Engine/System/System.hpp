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
    render
};



// Systems are static classes, and static classes can't be polymorphic, but every
// system should resemble the following...
/*
class System {

    friend Scene;

    public:

    static void init();

    static void update(float dt) = 0;

    private:

    static void add(std::unique_ptr<Component> component);

    static void remove(Component * component);

};
*/



#endif