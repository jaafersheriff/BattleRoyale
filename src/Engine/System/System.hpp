/* System interface */
#pragma once
#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_



#include <vector>
#include <memory>

#include "Component/Component.hpp"



class System {

    public:

        System(const std::vector<Component *> & components);

        /* virtual destructor necessary for polymorphic destruction */
        virtual ~System() = default;

        /* Generic update function */
        virtual void update(float dt);

        const std::vector<Component *> & components() const { return m_components; }

    protected:

        /* Reference to components of this system */
        const std::vector<Component *> & m_components;

};



#endif