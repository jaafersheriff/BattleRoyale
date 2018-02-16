#include "CollisionSystem.hpp"

#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/norm.hpp"

#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CollisionComponents/CollisionComponent.hpp"
#include "Scene/Scene.hpp"



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



bool collide(BounderComponent & b1, BounderComponent & b2, std::unordered_map<BounderComponent *, std::vector<std::pair<int, glm::vec3>>> * collisions) {
    if (b1.weight() == UINT_MAX && b2.weight() == UINT_MAX) {
        return false;
    }
    if (!collisions) {
        return b1.collide(b2, nullptr);
    }

    if (b1.weight() == 0 || b2.weight() == 0) {
        bool res(b1.collide(b2, nullptr));
        if (res) {
            if (b1.weight() != UINT_MAX) (*collisions)[&b1];
            if (b2.weight() != UINT_MAX) (*collisions)[&b2];
        }
        return res;
    }
    
    glm::vec3 delta;
    if (!b1.collide(b2, &delta)) {
        return false;
    }    
    if (b1.weight() < b2.weight()) {
        (*collisions)[&b1].push_back({ b2.weight(), delta });
        if (b2.weight() != UINT_MAX) (*collisions)[&b2];
    }
    else if (b2.weight() < b1.weight()) {
        if (b1.weight() != UINT_MAX) (*collisions)[&b1];
        (*collisions)[&b2].push_back({ b1.weight(), -delta });
    }
    else {
        delta *= 0.5f;
        (*collisions)[&b1].push_back({ b2.weight(), delta });
        (*collisions)[&b2].push_back({ b1.weight(), -delta });
    }

    return true;
}

// combines two adjustment deltas such that the maximum of each component is preserved
glm::vec3 compositeDeltas(const glm::vec3 & d1, const glm::vec3 & d2) {
    glm::vec3 d;

    if      (d1.x > 0 && d2.x > 0) d.x = glm::max(d1.x, d2.x);
    else if (d1.x < 0 && d2.x < 0) d.x = glm::min(d1.x, d2.x);
    else                           d.x = d1.x + d2.x;

    if      (d1.y > 0 && d2.y > 0) d.y = glm::max(d1.y, d2.y);
    else if (d1.y < 0 && d2.y < 0) d.y = glm::min(d1.y, d2.y);
    else                           d.y = d1.y + d2.y;

    if      (d1.z > 0 && d2.z > 0) d.z = glm::max(d1.z, d2.z);
    else if (d1.z < 0 && d2.z < 0) d.z = glm::min(d1.z, d2.z);
    else                           d.z = d1.z + d2.z;
    
    return d;
}

// compares pairs of weight and delta by weight for std::sort
bool compWeightDelta(const std::pair<int, glm::vec3> & d1, const std::pair<int, glm::vec3> & d2) {
    return d1.first < d2.first;
}

// norm must be unit vector
// If v dot norm is positive, just return v. Otherwise, return the closest
// point on the plane defined by norm.
glm::vec3 hemiClamp(const glm::vec3 & v, const glm::vec3 & norm) {
    float dot(glm::dot(v, norm));
    if (dot >= 0) {
        return v;
    }
    return v - dot * norm;
}

// Computes the net delta from the given deltas mapped by weight
// A nieve approach would be to simply add all deltas together.
// This breaks down when introducing the concept of weight. A delta of a higher
// weight takes precidence. But this doesn't mean you ignore the lower weight
// delta. Rather, you "hemispherically clamp" the net lower weight delta
// onto each higher weight delta, and repeat this process, moving up in weight.
glm::vec3 detNetDelta(std::vector<std::pair<int, glm::vec3>> & weightDeltas) {
    if (weightDeltas.size() == 0) {
        return glm::vec3();
    }

    std::sort(weightDeltas.begin(), weightDeltas.end(), compWeightDelta);
    
    glm::vec3 net;
    unsigned int weightI(0);
    unsigned int i(weightI);
    unsigned int weight(weightDeltas[weightI].first);
    glm::vec3 weightDelta;
    for (; i < weightDeltas.size() && weightDeltas[i].first == weight; ++i) {
        weightDelta = compositeDeltas(weightDelta, weightDeltas[i].second);
    }
    net = compositeDeltas(net, weightDelta);
    weightI = i;
    while (weightI < weightDeltas.size()) {
        weight = weightDeltas[weightI].first;
        weightDelta = glm::vec3();
        for (; i < weightDeltas.size() && weightDeltas[i].first == weight; ++i) {
            const glm::vec3 & delta(weightDeltas[i].second);
            weightDelta = compositeDeltas(weightDelta, delta);
            net = hemiClamp(net, glm::normalize(delta));
        }
        net = compositeDeltas(net, weightDelta);
        weightI = i;
    }

    return net;
}



}



void CollisionSystem::init() {
    auto spatTransformCallback(
        [&](const Message & msg_) {
            const SpatialTransformTag & msg(static_cast<const SpatialTransformTag &>(msg_));
            for (auto & bounder : msg.spatial.getGameObject()->getComponentsBySystem(SystemID::collision)) {
                m_potentials.insert(static_cast<BounderComponent *>(bounder));
            }
        }
    );
    Scene::get().addReceiver<SpatialPositionSetMessage>(nullptr, spatTransformCallback);
    Scene::get().addReceiver<SpatialMovedMessage>(nullptr, spatTransformCallback);
    Scene::get().addReceiver<SpatialScaleSetMessage>(nullptr, spatTransformCallback);
    Scene::get().addReceiver<SpatialScaledMessage>(nullptr, spatTransformCallback);
    Scene::get().addReceiver<SpatialOrientationSetMessage>(nullptr, spatTransformCallback);
    Scene::get().addReceiver<SpatialRotatedMessage>(nullptr, spatTransformCallback);
}

void CollisionSystem::update(float dt) {
    static std::unordered_map<BounderComponent *, std::vector<std::pair<int, glm::vec3>>> s_collisions;
    static std::unordered_set<BounderComponent *> s_checked;
    static std::unordered_map<GameObject *, glm::vec3> s_gameObjectDeltas;

    m_collided.clear();
    m_adjusted.clear();

    // gather all collisions
    for (BounderComponent * bounder : m_potentials) {
        bounder->update(dt);
        s_checked.insert(bounder);
        for (auto & other : m_bounderComponents) {
            if (s_checked.count(other.get()) || other->getGameObject() == bounder->getGameObject()) {
                continue;
            }
            if (collide(*bounder, *other, &s_collisions)) {
                Scene::get().sendMessage<CollisionMessage>(bounder->getGameObject(), *bounder, *other);
                Scene::get().sendMessage<CollisionMessage>(other->getGameObject(), *other, *bounder);
            }
        }
    }
    
    // composite deltas into a single delta per game object
    for (auto & pair : s_collisions) {
        BounderComponent & bounder(*pair.first);
        auto & weightDeltas(pair.second);
        m_collided.insert(&bounder);
        // there was an adjustment
        if (weightDeltas.size()) {
            glm::vec3 & gameObjectDelta(s_gameObjectDeltas[bounder.getGameObject()]);
            gameObjectDelta = compositeDeltas(gameObjectDelta, detNetDelta(weightDeltas));
        }
    }

    // apply deltas to game objects
    for (auto & pair : s_gameObjectDeltas) {
        GameObject * gameObject(pair.first);
        SpatialComponent & spat(*gameObject->getSpatial());
        const glm::vec3 & delta(pair.second);
        // set position rather than move because they are conceptually different
        // this will come into play if we do time step interpolation
        spat.setPosition(spat.position() + delta + glm::normalize(delta) * k_collisionOffsetFactor);
        for (Component * comp : gameObject->getComponentsBySystem(SystemID::collision)) {
            BounderComponent * bounder(static_cast<BounderComponent *>(comp));
            bounder->update(dt);
            m_adjusted.insert(bounder);
            Scene::get().sendMessage<CollisionAdjustMessage>(gameObject, *gameObject);
        }
    }
    
    m_potentials.clear();
    s_checked.clear();
    s_collisions.clear();
    s_gameObjectDeltas.clear();
}

Intersect CollisionSystem::pick(const Ray & ray) const {
    Intersect inter;
    for (const auto & bounder : m_bounderComponents) {
        Intersect potential(bounder->intersect(ray));
        if (potential.dist < inter.dist) {
            inter = potential;
        }
    }
    return inter;
}

void CollisionSystem::add(std::unique_ptr<Component> component) {
    m_componentRefs.push_back(component.get());
    if (dynamic_cast<BounderComponent *>(component.get())) {
        m_bounderComponents.emplace_back(static_cast<BounderComponent *>(component.release()));
        m_bounderComponents.back()->update(0.0f);
    }
    else {
        assert(false);
    }
}

void CollisionSystem::remove(Component * component) {
    if (dynamic_cast<BounderComponent *>(component)) {
        for (auto it(m_bounderComponents.begin()); it != m_bounderComponents.end(); ++it) {
            if (it->get() == component) {
                m_bounderComponents.erase(it);
                break;
            }
        }
    }
    // remove from refs
    for (auto it(m_componentRefs.begin()); it != m_componentRefs.end(); ++it) {
        if (*it == component) {
            m_componentRefs.erase(it);
            break;
        }
    }
}