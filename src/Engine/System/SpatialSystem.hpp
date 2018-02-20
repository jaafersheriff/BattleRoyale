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

    private:

    static void add(std::unique_ptr<Component> component);

    static void remove(Component * component);

    private:

    static std::vector<std::unique_ptr<SpatialComponent>> s_spatialComponents;
    static std::vector<std::unique_ptr<NewtonianComponent>> s_newtonianComponents;
    static std::vector<std::unique_ptr<AcceleratorComponent>> s_acceleratorComponents;
    static glm::vec3 s_gravityDir;
    static float s_gravityMag;
    static float s_coefficientOfFriction;

};