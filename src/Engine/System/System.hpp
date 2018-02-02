/* System interface */
#pragma once
#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include <vector>

class System {
    public:
        /* List of component interface that this system is responsible for */
        template <class T>
        std::vector<T> components;

        /* Generic update function */
        virtual void update(float dt) = 0;
};

#endif