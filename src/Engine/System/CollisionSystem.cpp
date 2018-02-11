#include "CollisionSystem.hpp"

#include <unordered_set>
#include <unordered_map>

#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/norm.hpp"

#include "Component/CollisionComponents/CollisionComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"



namespace {



struct Collision {

    BounderComponent * b1, * b2;
    glm::vec3 d1, d2;

    Collision() :
        b1(nullptr), b2(nullptr),
        d1(), d2()
    {}

    Collision(BounderComponent * b1, BounderComponent * b2, const glm::vec3 & d1, const glm::vec3 & d2) :
        b1(b1), b2(b2),
        d1(d1), d2(d2)
    {}

};



bool collide(BounderComponent & b1, BounderComponent & b2, std::unordered_map<BounderComponent *, glm::vec3> * collisions) {
    glm::vec3 delta;
    if (!b1.collide(b2, &delta)) {
        return false;
    }
    
    if (b1.weight() < b2.weight()) {
        (*collisions)[&b1] += delta;
    }
    else if (b2.weight() < b1.weight()) {
        (*collisions)[&b2] -= delta;
    }
    else {
        delta *= 0.5f;
        (*collisions)[&b1] += delta;
        (*collisions)[&b2] -= delta;
    }

    return true;
}



}



CollisionSystem::CollisionSystem(const std::vector<Component *> & comps) :
    System(comps)
{}

void CollisionSystem::update(float dt) {
    static std::unordered_map<BounderComponent *, glm::vec3> s_collisions;
    static std::unordered_set<BounderComponent *> s_checked;

    for (auto & comp : m_components) {
        comp->update(dt);
    }

    for (auto & bounder_ : m_components) {
        BounderComponent & bounder(*static_cast<BounderComponent *>(bounder_));
        SpatialComponent * spat(bounder.getGameObject()->getSpatial());
        if (!spat || !spat->transformed()) {
            continue;
        }
        s_checked.insert(&bounder);
        for (auto & other_ : m_components) {
            BounderComponent & other(*static_cast<BounderComponent *>(other_));
            if (s_checked.count(&other) || other.getGameObject() == bounder.getGameObject()) {
                continue;
            }
            if (collide(bounder, other, &s_collisions)) {

            }
        }
    }
    
    for (auto & pair : s_collisions) {
        BounderComponent & bounder(*pair.first);
        glm::vec3 & delta(pair.second);
        if (!Util::isZero(delta)) {
            SpatialComponent & spat(*bounder.getGameObject()->getSpatial());
            // set position rather than move because they are conceptually different
            // this will come into play if we do time step interpolation
            spat.setPosition(spat.position() + delta + glm::normalize(delta) * k_collisionOffsetFactor);
            bounder.update();
            bounder.m_isCollision = true;
        }
    }

    s_checked.clear();
    s_collisions.clear();
}

Intersect CollisionSystem::pick(const Ray & ray) const {
    Intersect inter;
    for (const auto & bounder_ : m_components) {
        const BounderComponent & bounder(*static_cast<const BounderComponent *>(bounder_));
        Intersect potential(bounder.intersect(ray));
        if (potential.dist < inter.dist) {
            inter = potential;
        }
    }
    return inter;
}