/* System interface */
#pragma once
#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include <vector>
#include <memory>

#include "Component/Component.hpp"

class Scene;

class System {

    protected: // only scene can create system

        System(const std::vector<Component *> & components);

        // TODO: potentially add move support
        System(const System & other) = delete; // doesn't make sense to copy system
        System & operator=(const System & other) = delete;

    public:

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