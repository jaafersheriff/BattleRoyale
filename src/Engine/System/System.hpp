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

        /* Generic update function */
        virtual void update(float dt);

    protected:

        /* Reference to components of this system */
        const std::vector<Component *> & components;

};

#endif