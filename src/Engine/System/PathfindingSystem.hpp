#pragma once

#include "System.hpp"
#include "Util/Memory.hpp"

class PathfindingComponent;
class Scene;

// static class
class PathfindingSystem {

    friend Scene;
    
    public:

    static void init() {}

    static void update(float dt);

    private:

    static const Vector<PathfindingComponent *> & s_pathfindingComponents;

};