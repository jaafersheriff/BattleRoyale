#pragma once



#include "glm/glm.hpp"

#include "System.hpp"
#include "Component/PostCollisionComponents/GroundComponent.hpp"



// static class
class PostCollisionSystem {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::postCollision;

    public:

    static void init() {};

    static void update(float dt);

    private:

    static const Vector<GroundComponent *> & s_groundComponents;

};