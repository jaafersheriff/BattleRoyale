#pragma once



#include "glm/glm.hpp"

#include "System.hpp"
#include "Util/Memory.hpp"



class Scene;
class SpatialComponent;
class NewtonianComponent;
class AcceleratorComponent;
class AnimationComponent;



// static class
class SpatialSystem {

    friend Scene;

    public:

    static void init();

    static void update(float dt);

    static void setGravity(const glm::vec3 & gravity);
    static void setGravityDir(const glm::vec3 & dir);
    static void setGravityMag(float mag);

    static glm::vec3 gravity() { return s_gravityDir * s_gravityMag; }
    static const glm::vec3 & gravityDir() { return s_gravityDir; }
    static float gravityMag() { return s_gravityMag; }

    public:

    static const float k_terminalVelocity;
    static const float k_coefficientOfFriction;
    static const float k_elasticity;
    static const float k_bounceVelThreshold;

    private:

    static const Vector<SpatialComponent *> & s_spatialComponents;
    static const Vector<NewtonianComponent *> & s_newtonianComponents;
    static const Vector<AcceleratorComponent *> & s_acceleratorComponents;
    static const Vector<AnimationComponent *> & s_animationComponents;
    static glm::vec3 s_gravityDir;
    static float s_gravityMag;

};