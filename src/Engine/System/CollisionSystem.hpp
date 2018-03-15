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

    // Casts a ray and returns the first bounder hit and its intersection
    static std::pair<const BounderComponent *, Intersect> pick(const Ray & ray);
    // Only bounders which pass the conditional are considered
    static std::pair<const BounderComponent *, Intersect> pick(const Ray & ray, const std::function<bool(const BounderComponent &)> & conditional);
    // Ray will pass through bounders with weight less than specified and store them in r_passed, if not null
    static std::pair<const BounderComponent *, Intersect> pickHeavy(
        const Ray & ray,
        unsigned int minWeight,
        Vector<const BounderComponent *> * r_passed = nullptr,
        float maxDist = Util::infinity()
    );
    static std::pair<const BounderComponent *, Intersect> pickHeavy(
        const Ray & ray,
        unsigned int minWeight,
        const std::function<bool(const BounderComponent &)> & conditional,
        Vector<const BounderComponent *> * r_passed = nullptr,
        float maxDist = Util::infinity()
    );

    static void setOctree(const glm::vec3 & min, const glm::vec3 & max, float minCellSize);

    static void remakeOctree();

    // chooses the bounder with the smallest volume from the vertex data of the given mesh
    // optionally enable/disable certain types of bounders. If all are false you are
    // dumb and it acts as if all were true
    static BounderComponent & addBounderFromMesh(GameObject & gameObject, unsigned int weight, const Mesh & mesh, bool allowAAB, bool allowSphere, bool allowCapsule);

    private:

    static const Vector<BounderComponent *> & s_bounderComponents;
    static UnorderedSet<BounderComponent *> s_potentials;
    static UnorderedSet<const BounderComponent *> s_collided;
    static UnorderedSet<const BounderComponent *> s_adjusted;
    static UniquePtr<Octree<const BounderComponent *>> s_octree;

};