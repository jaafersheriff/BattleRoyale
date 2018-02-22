#pragma once



#include <unordered_set>

#include "System.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"



struct Ray;
struct Intersect;
class BounderShader;



// static class
class CollisionSystem {

    friend Scene;
    friend BounderShader;

    public:

    static constexpr SystemID ID = SystemID::collision;

    static void init();

    static void update(float dt);

    static std::pair<BounderComponent *, Intersect> pick(const Ray & ray);

    // chooses the bounder with the smallest volume from the vertex data of the given mesh
    // optionally enable/disable certain types of bounders. If all are false you are
    // dumb and it acts as if all were true
    static BounderComponent & addBounderFromMesh(GameObject & gameObject, unsigned int weight, const Mesh & mesh, bool allowAAB, bool allowSphere, bool allowCapsule);

    private:
    
    static void added(Component & component);

    static void removed(Component & component);

    private:

    static const std::vector<BounderComponent *> & s_bounderComponents;
    static std::unordered_set<BounderComponent *> s_potentials;
    static std::unordered_set<BounderComponent *> s_collided;
    static std::unordered_set<BounderComponent *> s_adjusted;

};