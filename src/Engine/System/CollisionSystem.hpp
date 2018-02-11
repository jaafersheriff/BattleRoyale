#pragma once



#include "System.hpp"



struct Ray;



class CollisionSystem : public System {

    public:

    constexpr static Type type = COLLISION;

    static constexpr float k_collisionOffsetFactor = 0.01f;

    public:

    CollisionSystem(const std::vector<Component *> & comps);

    virtual void update(float dt) override;

    Intersect pick(const Ray & ray) const;

};