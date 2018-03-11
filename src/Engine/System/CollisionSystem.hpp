#pragma once



#include "System.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"



struct Ray;
struct Intersect;
class BounderShader;
template <typename T> class Octree;
class OctreeShader;



// static class
class CollisionSystem {

    friend Scene;
    friend BounderShader;
    friend OctreeShader;

    public:

    static constexpr SystemID ID = SystemID::collision;

    public:

    static void init();

    static void update(float dt);

    static std::pair<const BounderComponent *, Intersect> pick(const Ray & ray, const GameObject * ignore);
    static std::pair<const BounderComponent *, Intersect> pick(const Ray & ray, const std::function<bool(const BounderComponent &)> & conditional);

    static void setOctree(const glm::vec3 & min, const glm::vec3 & max, float minCellSize);

    // chooses the bounder with the smallest volume from the vertex data of the given mesh
    // optionally enable/disable certain types of bounders. If all are false you are
    // dumb and it acts as if all were true
    static BounderComponent & addBounderFromMesh(GameObject & gameObject, unsigned int weight, const Mesh & mesh, bool allowAAB, bool allowSphere, bool allowCapsule);

    private:

    static const Vector<BounderComponent *> & s_bounderComponents;
    static UnorderedSet<BounderComponent *> s_potentials;
    static UnorderedSet<const BounderComponent *> s_collided;
    static UnorderedSet<const BounderComponent *> s_adjusted;
    static UniquePtr<Octree<BounderComponent>> s_octree;

};