#pragma once



#include "System.hpp"
#include "Component/CollisionComponents/CollisionComponent.hpp"



struct Ray;
struct Intersect;
class BounderShader;



// Singleton
class CollisionSystem : public System {

    friend Scene;
    friend BounderShader;

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

    ~CollisionSystem() = default;

    virtual void init() override {}

    virtual void update(float dt) override;

    Intersect pick(const Ray & ray) const;

    private:
    
    virtual void add(std::unique_ptr<Component> component) override;

    virtual void remove(Component * component) override;

    private:

    std::vector<std::unique_ptr<BounderComponent>> m_bounderComponents;

};