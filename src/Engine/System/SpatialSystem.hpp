#pragma once

#include "System.hpp"

class SpatialSystem : public System {

    public:

    constexpr static Type type = SPATIAL;

    public:

    SpatialSystem(const std::vector<Component *> & comps);

};