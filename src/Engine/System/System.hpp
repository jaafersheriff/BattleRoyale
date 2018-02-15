/* System interface */
#pragma once
#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_



#include <vector>
#include <memory>



class Scene;
class Component;
class GameObject;



enum class SystemID {
    gameLogic,
    pathfinding,
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

    const std::vector<Component *> components() const { return m_componentRefs; }

    private:

    virtual void add(std::unique_ptr<Component> component) = 0;

    virtual void remove(Component * component) = 0;

    protected:

    std::vector<Component *> m_componentRefs;

};



#endif