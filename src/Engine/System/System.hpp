/* System interface */
#pragma once
#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_



#include <vector>
#include <memory>



class Scene;
class Component;



enum class SystemID {
    gameLogic,
    spatial,
    collision,
    render
};



// Singleton
class System {

    friend Scene;

    protected:

    System() = default;

    System(const System & other) = delete;
    System & operator=(const System & other) = delete;

    public:

    virtual ~System() = default;

    virtual void init() = 0;

    virtual void update(float dt) = 0;

    void add(std::unique_ptr<Component> component);

    void remove(Component * component);

};



#endif