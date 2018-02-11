/* System interface */
#pragma once
#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_



#include <vector>
#include <memory>

#include "Component/Component.hpp"



class System {

    public:

    enum Type {
        GAMELOGIC,
        RENDER,
        SPATIAL,
        COLLISION
    };

    public:

        System(const std::vector<Component *> & components);

        /* virtual destructor necessary for polymorphic destruction */
        virtual ~System() = default;

        /* Generic update function */
        virtual void update(float dt);

    protected:

        /* Reference to components of this system */
        const std::vector<Component *> & m_components;

};



#endif