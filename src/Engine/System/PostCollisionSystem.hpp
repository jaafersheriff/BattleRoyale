#pragma once



#include "glm/glm.hpp"

#include "System.hpp"
#include "Util/Memory.hpp"



class Scene;
class GroundComponent;



// static class
class PostCollisionSystem {

    friend Scene;

    public:

    static void init() {};

    static void update(float dt);

    private:

    static const Vector<GroundComponent *> & s_groundComponents;

};