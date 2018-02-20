#pragma once



#include <unordered_set>

#include "System.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"



struct Ray;
struct Intersect;
class BounderShader;



// Singleton
class CollisionSystem : public System {

    friend Scene;
    friend BounderShader;

    public:

    static constexpr SystemID ID = SystemID::collision;

    public:

    static CollisionSystem & get() {
        static CollisionSystem s_collisionSystem;
        return s_collisionSystem;
    }

    private:

    CollisionSystem() = default;

    public:

    ~CollisionSystem() = default;

    virtual void init() override;

    virtual void update(float dt) override;

    std::pair<BounderComponent *, Intersect> pick(const Ray & ray) const;

    // chooses the bounder with the smallest volume from the vertex data of the given mesh
    // optionally enable/disable certain types of bounders. If all are false you are
    // dumb and it acts as if all were true
    BounderComponent & addBounderFromMesh(GameObject & gameObject, unsigned int weight, const Mesh & mesh, bool allowAAB, bool allowSphere, bool allowCapsule) const;

    private:
    
    virtual void add(std::unique_ptr<Component> component) override;

    virtual void remove(Component * component) override;

    void updateBounders(float dt);

    private:

    std::vector<std::unique_ptr<BounderComponent>> m_bounderComponents;
    std::unordered_set<BounderComponent *> m_potentials;
    std::unordered_set<BounderComponent *> m_collided;
    std::unordered_set<BounderComponent *> m_adjusted;

};