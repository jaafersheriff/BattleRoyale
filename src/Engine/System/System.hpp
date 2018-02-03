/* System interface */
#pragma once
#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include "Component/Component.hpp"

#include <vector>

class System {
    public:
        void addComponent(Component *cp) {
            this->components.push_back(cp);
        }

        /* Generic update function */
        virtual void update(float dt) = 0;
    protected:
        /* List of component interface that this system is responsible for */
        std::vector<Component *> components;
};

#endif