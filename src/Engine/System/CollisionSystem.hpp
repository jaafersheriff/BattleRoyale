#pragma once



#include "System.hpp"



struct Ray;
struct Intersect;



class CollisionSystem : public System {

    friend Scene;

    static constexpr float k_collisionOffsetFactor = 0.01f;

    private: // only scene can create system

    CollisionSystem(const std::vector<Component *> & comps);

    public:

    virtual void update(float dt) override;

    Intersect pick(const Ray & ray) const;

};