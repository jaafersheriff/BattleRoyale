#pragma once



#include "System.hpp"
#include "Component/CollisionComponents/CollisionComponent.hpp"



struct Ray;
struct Intersect;



// Singleton
class CollisionSystem : public System {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::collision;

    static constexpr float k_collisionOffsetFactor = 0.01f;

    public:

    static CollisionSystem & get() {
        static CollisionSystem s_collisionSystem;
        return s_collisionSystem;
    }

    private:

    CollisionSystem() = default;

    public:

    virtual void init() override {}

    virtual void update(float dt) override;
    
    virtual void add(std::unique_ptr<Component> component) override;

    Intersect pick(const Ray & ray) const;

    private:

    std::vector<std::unique_ptr<BounderComponent>> m_bounderComponents;

};