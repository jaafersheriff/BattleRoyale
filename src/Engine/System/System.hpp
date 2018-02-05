/* System interface */
#pragma once
#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include "Component/Component.hpp"

#include <vector>

class System {
    public:
        System(std::vector<Component *> *components) {
            this->components = components;
        }

        /* Generic update function */
        virtual void update(float dt) = 0;
    protected:
        /* List of component interface that this system is responsible for */
        std::vector<Component *> *components;
};

#endif