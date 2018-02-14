#pragma once

#include "System.hpp"

class SpatialSystem : public System {

    friend Scene;

    private: // only scene can create system

    SpatialSystem(const std::vector<Component *> & comps);

    virtual void update(float dt) override;

};