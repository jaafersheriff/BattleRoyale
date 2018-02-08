#pragma once



#include "System/System.hpp"



class CollisionSystem : public System {

    public:

    CollisionSystem(std::vector<Component *> * cp) :
        System(cp)
    {}

    virtual void update(float dt) override;

};