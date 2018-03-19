#pragma once

#include "System.hpp"
#include "Component/MapExploreComponents/MapExploreComponent.hpp"
// static class
class MapExploreSystem {

    friend Scene;
    
    public:

    static void init() {}

    static void update(float dt);

    private:

    static const Vector<MapExploreComponent *> & s_mapexploreComponents;

};