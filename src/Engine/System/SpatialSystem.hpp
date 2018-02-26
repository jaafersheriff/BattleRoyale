#pragma once



#include "glm/glm.hpp"

#include "System.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/SpatialComponents/PhysicsComponents.hpp"



// static class
class SpatialSystem {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::spatial;

    public:

    static void init() {};

    static void update(float dt);

    static void setGravity(const glm::vec3 & gravity);
    static void setGravityDir(const glm::vec3 & dir);
    static void setGravityMag(float mag);

    static glm::vec3 gravity() { return s_gravityDir * s_gravityMag; }
    static const glm::vec3 & gravityDir() { return s_gravityDir; }
    static float gravityMag() { return s_gravityMag; }
    static float coefficientOfFriction() { return s_coefficientOfFriction; }
    static float terminalVelocity() { return s_terminalVelocity; }

    private:

    static void added(Component & component) {};

    static void removed(Component & component) {};

    private:

    static const Vector<SpatialComponent *> & s_spatialComponents;
    static const Vector<NewtonianComponent *> & s_newtonianComponents;
    static const Vector<AcceleratorComponent *> & s_acceleratorComponents;
    static glm::vec3 s_gravityDir;
    static float s_gravityMag;
    static const float s_coefficientOfFriction;
    static const float s_terminalVelocity;

};