#include "CollisionSystem.hpp"

#include <algorithm>

#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/norm.hpp"

#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Scene/Scene.hpp"
#include "Util/Octree.hpp"



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



bool collide(const BounderComponent & b1, const BounderComponent & b2, UnorderedMap<const BounderComponent *, Vector<std::pair<int, glm::vec3>>> * collisions) {
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
        (*collisions)[&b1].push_back(std::make_pair(b2.weight(), delta));
        if (b2.weight() != UINT_MAX) (*collisions)[&b2];
    }
    else if (b2.weight() < b1.weight()) {
        if (b1.weight() != UINT_MAX) (*collisions)[&b1];
        (*collisions)[&b2].push_back(std::make_pair(b1.weight(), -delta));
    }
    else {
        delta *= 0.5f;
        (*collisions)[&b1].push_back(std::make_pair(b2.weight(), delta));
        (*collisions)[&b2].push_back(std::make_pair(b1.weight(), -delta));
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

// Computes the net delta from the given deltas mapped by weight
// A nieve approach would be to simply add all deltas together.
// This breaks down when introducing the concept of weight. A delta of a higher
// weight takes precidence. But this doesn't mean you ignore the lower weight
// delta. Rather, you "hemispherically clamp" the net lower weight delta
// onto each higher weight delta, and repeat this process, moving up in weight.
glm::vec3 detNetDelta(Vector<std::pair<int, glm::vec3>> & weightDeltas) {
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
            net = Util::removeAllAgainst(net, Util::safeNorm(delta));
        }
        net = compositeDeltas(net, weightDelta);
        weightI = i;
    }

    return net;
}



}



const Vector<BounderComponent *> & CollisionSystem::s_bounderComponents(Scene::getComponents<BounderComponent>());
UnorderedSet<BounderComponent *> CollisionSystem::s_potentials;
UnorderedSet<const BounderComponent *> CollisionSystem::s_collided;
UnorderedSet<const BounderComponent *> CollisionSystem::s_adjusted;
UniquePtr<Octree<BounderComponent>> CollisionSystem::s_octree;

void CollisionSystem::init() {
    auto compAddedCallback(
        [&](const Message & msg_) {
            const ComponentAddedMessage & msg(static_cast<const ComponentAddedMessage &>(msg_));            
            if (msg.typeI == typeid(BounderComponent)) {
                BounderComponent & bounder(static_cast<BounderComponent &>(msg.comp));
                s_potentials.insert(&bounder);
            }
        }
    );
    Scene::addReceiver<ComponentAddedMessage>(nullptr, compAddedCallback);

    auto compRemovedCallback(
        [&](const Message & msg_) {
            const ComponentRemovedMessage & msg(static_cast<const ComponentRemovedMessage &>(msg_));            
            if (msg.typeI == typeid(BounderComponent)) {
                BounderComponent & bounder(const_cast<BounderComponent &>(static_cast<const BounderComponent &>(*msg.comp)));
                s_potentials.erase(&bounder);
                if (s_octree) s_octree->remove(bounder);
            }
        }
    );
    Scene::addReceiver<ComponentRemovedMessage>(nullptr, compRemovedCallback);

    auto spatTransformCallback(
        [&](const Message & msg_) {
            const SpatialTransformTag & msg(static_cast<const SpatialTransformTag &>(msg_));
            for (auto & comp : msg.spatial.gameObject().getComponentsByType<BounderComponent>()) {
                BounderComponent & bounder(static_cast<BounderComponent &>(*comp));
                s_potentials.insert(&bounder);
            }
        }
    );
    Scene::addReceiver<SpatialPositionSetMessage>(nullptr, spatTransformCallback);
    Scene::addReceiver<SpatialMovedMessage>(nullptr, spatTransformCallback);
    Scene::addReceiver<SpatialScaleSetMessage>(nullptr, spatTransformCallback);
    Scene::addReceiver<SpatialScaledMessage>(nullptr, spatTransformCallback);
    Scene::addReceiver<SpatialOrientationSetMessage>(nullptr, spatTransformCallback);
    Scene::addReceiver<SpatialRotatedMessage>(nullptr, spatTransformCallback);
}

void CollisionSystem::update(float dt) {
    static UnorderedMap<const BounderComponent *, Vector<std::pair<int, glm::vec3>>> s_collisions;
    static UnorderedSet<BounderComponent *> s_criticals;
    static UnorderedSet<const BounderComponent *> s_checked;
    static UnorderedMap<const GameObject *, glm::vec3> s_gameObjectDeltas;
    static Vector<const BounderComponent *> s_octreeResults;
    static UnorderedSet<GameObject *> s_outOfBounds;

    s_collided.clear();
    s_adjusted.clear();

    // update all potential bounders
    for (BounderComponent * bounder : s_potentials) {
        bounder->update(dt);
    }

    // update octree
    if (s_octree) {
        for (BounderComponent * bounder : s_potentials) {
            if (!s_octree->set(*bounder, bounder->enclosingAABox())) {
                s_outOfBounds.insert(&bounder->gameObject());
            }
        }
        // remove all out of bounds game objects
        for (GameObject * go : s_outOfBounds) {
            const auto & bounders(go->getComponentsByType<BounderComponent>());
            for (BounderComponent * bounder : bounders) {
                s_potentials.erase(bounder);
            }
            Scene::destroyGameObject(*go);
        }
        s_outOfBounds.clear();
    }
    
    // determine all bounders with path intersections
    for (BounderComponent * bounder : s_potentials) {
        if (bounder->isCritical()) {
            s_criticals.insert(bounder);
            s_gameObjectDeltas[&bounder->gameObject()];
        }
    }
    // determine path intersection corrections per game object
    for (BounderComponent * bounder : s_criticals) {
        glm::vec3 delta(bounder->center() - bounder->prevCenter());
        auto pair(pick(
            Ray(bounder->prevCenter(), glm::normalize(delta)),
            [bounder](const BounderComponent & b) {
                return &b.gameObject() != &bounder->gameObject() && !s_gameObjectDeltas.count(&const_cast<BounderComponent &>(b).gameObject());
            }
        ));
        const Intersect & inter(pair.second);
        if (inter.is && inter.dist * inter.dist < glm::length2(delta)) {
            glm::vec3 & d(s_gameObjectDeltas.at(&bounder->gameObject()));
            d = compositeDeltas(d, inter.pos - bounder->center());
        }
    }
    // apply path intersection corrections
    for (auto & pair : s_gameObjectDeltas) {
        if (pair.second == glm::vec3()) {
            continue;
        }
        const GameObject & go(*pair.first);
        SpatialComponent & spat(*go.getSpatial());
        spat.setPosition(spat.position() + pair.second, true);
        for (Component * comp : go.getComponentsByType<BounderComponent>()) {
            BounderComponent & bounder(static_cast<BounderComponent &>(*comp));
            s_potentials.insert(&bounder);
            bounder.update(dt);
            if (s_octree) {
                s_octree->set(bounder, bounder.enclosingAABox());
            }
        }
    }
    s_criticals.clear();
    s_gameObjectDeltas.clear();
    
    // gather all collisions
    for (BounderComponent * bounder : s_potentials) {
        s_checked.insert(bounder);
        const Vector<const BounderComponent *> * possible(&reinterpret_cast<const Vector<const BounderComponent *> &>(s_bounderComponents));
        if (s_octree) {
            s_octree->filter(*bounder, s_octreeResults);
            possible = &s_octreeResults;
        }
        for (const BounderComponent * other : *possible) {
            if (s_checked.count(other) || &other->gameObject() == &bounder->gameObject()) {
                continue;
            }
            if (collide(*bounder, *other, &s_collisions)) {
                Scene::sendMessage<CollisionMessage>(&bounder->gameObject(), *bounder, *other);
                Scene::sendMessage<CollisionMessage>(&other->gameObject(), *other, *bounder);
            }
        }
        s_octreeResults.clear();
    }
    s_potentials.clear();
    s_checked.clear();    
    
    // composite deltas into a single delta per game object
    // additionally send norm messages
    for (auto & pair : s_collisions) {
        const BounderComponent & bounder(*pair.first);
        auto & weightDeltas(pair.second);
        s_collided.insert(&bounder);
        // there was an adjustment
        if (weightDeltas.size()) {
            for (auto & weightDelta : weightDeltas) { // send norm messages
                Scene::sendMessage<CollisionNormMessage>(&bounder.gameObject(), bounder, Util::safeNorm(weightDelta.second));
            }
            glm::vec3 & gameObjectDelta(s_gameObjectDeltas[&bounder.gameObject()]);
            gameObjectDelta = compositeDeltas(gameObjectDelta, detNetDelta(weightDeltas));
        }
    }
    s_collisions.clear();

    // apply deltas to game objects
    for (auto & pair : s_gameObjectDeltas) {
        const GameObject * gameObject(pair.first);
        SpatialComponent & spat(*gameObject->getSpatial());
        const glm::vec3 & delta(pair.second);
        // set position rather than move because they are conceptually different
        // this will come into play if we do time step interpolation
        spat.setPosition(spat.position() + delta, true);
        for (Component * comp : gameObject->getComponentsByType<BounderComponent>()) {
            BounderComponent * bounder(static_cast<BounderComponent *>(comp));
            s_potentials.insert(bounder);
            bounder->update(dt);
            if (s_octree) {
                s_octree->set(*bounder, bounder->enclosingAABox());
            }
            s_adjusted.insert(bounder);
            Scene::sendMessage<CollisionAdjustMessage>(gameObject, *gameObject, delta);
        }
    }
    s_gameObjectDeltas.clear();
}

std::pair<const BounderComponent *, Intersect> CollisionSystem::pick(const Ray & ray, const GameObject * ignore) {
    return pick(ray, [ignore](const BounderComponent & bounder) { return &bounder.gameObject() != ignore; });
}

std::pair<const BounderComponent *, Intersect> CollisionSystem::pick(const Ray & ray, const std::function<bool(const BounderComponent &)> & conditional) {
    static Vector<BounderComponent *> s_octreeResults;

    if (s_octree) {
        return s_octree->filter(ray, [& conditional](const Ray & ray, const BounderComponent & bounder) {
            return conditional(bounder) ? bounder.intersect(ray) : Intersect();
        });
    }
    else {
        BounderComponent * bounder(nullptr);
        Intersect inter;
        for (BounderComponent * b : s_bounderComponents) {
            if (!conditional(*b)) continue;
            Intersect potential(b->intersect(ray));
            if (potential.dist < inter.dist) {
                bounder = b;
                inter = potential;
            }
        }
        return std::pair<BounderComponent *, Intersect>(bounder, inter);
    }

    /*
    const Vector<BounderComponent *> * possible(&s_bounderComponents);
    if (s_octree) {
        s_octree->filter(ray, s_octreeResults);
        possible = &s_octreeResults;
    }

    BounderComponent * bounder(nullptr);
    Intersect inter;
    for (BounderComponent * b : *possible) {
        if (!conditional(*b)) continue;
        Intersect potential(b->intersect(ray));
        if (potential.dist < inter.dist) {
            bounder = b;
            inter = potential;
        }
    }

    s_octreeResults.clear();

    return { bounder, inter };
    */
}

void CollisionSystem::setOctree(const glm::vec3 & min, const glm::vec3 & max, float minCellSize) {
    s_octree = UniquePtr<Octree<BounderComponent>>::make(AABox(min, max), minCellSize);
    for (BounderComponent * bounder : s_bounderComponents) {
        s_octree->set(*bounder, bounder->enclosingAABox());
    }
}

namespace {

std::pair<glm::vec3, glm::vec3> detMeshSpan(int nVerts, const glm::vec3 * positions) {    
    glm::vec3 min(Util::infinity()), max(-Util::infinity());
    for (int i(0); i < nVerts; ++i) {
        min = glm::min(min, positions[i]);
        max = glm::max(max, positions[i]);
    }
    return { min, max };
}

float detMaxRadius(int n, const glm::vec3 * positions, const glm::vec3 & center) {
    float maxR2(0.0f);
    for (int i(0); i < n; ++i) {
        float r2(glm::length2(positions[i] - center));
        if (r2 > maxR2) maxR2 = r2;
    }
    return std::sqrt(maxR2);
}

// returns min radius, absolute y upper, and absolute y lower
std::tuple<float, float, float> detCapsuleSpecs(int n, const glm::vec3 * positions, const glm::vec3 & center) {
    float maxR2(0.0f);
    for (int i(0); i < n; ++i) {
        float r2(glm::length2(glm::vec2(positions[i].x - center.x, positions[i].z - center.z)));
        if (r2 > maxR2) maxR2 = r2;
    }
    float r(std::sqrt(maxR2));

    float maxQy(-Util::infinity()), minQy(Util::infinity());
    for (int i(0); i < n; ++i) {
        float a(std::sqrt(maxR2 - glm::length2(glm::vec2(positions[i].x - center.x, positions[i].z - center.z))));
        if (positions[i].y >= center.y) {
            float qy(positions[i].y - a);
            if (qy > maxQy) maxQy = qy;
        }
        if (positions[i].y <= center.y) {
            float qy(positions[i].y + a);
            if (qy < minQy) minQy = qy;
        }
    }

    if (maxQy < minQy) {
        maxQy = minQy = (maxQy + minQy) * 0.5f;
    }

    return std::make_tuple(r, maxQy, minQy);
}

}

BounderComponent & CollisionSystem::addBounderFromMesh(GameObject & gameObject, unsigned int weight, const Mesh & mesh, bool allowAAB, bool allowSphere, bool allowCapsule) {
    if (!allowAAB && !allowSphere && !allowCapsule) {
        allowAAB = allowSphere = allowCapsule = true;
    }

    AABox box; Sphere sphere; Capsule capsule;
    float boxV(Util::infinity()), sphereV(Util::infinity()), capsuleV(Util::infinity());

    int nVerts(int(mesh.buffers.vertBuf.size()) / 3);
    const glm::vec3 * positions(reinterpret_cast<const glm::vec3 *>(mesh.buffers.vertBuf.data()));
    auto span(detMeshSpan(nVerts, positions));
    glm::vec3 & spanMin(span.first), & spanMax(span.second);
    glm::vec3 center((spanMax - spanMin) * 0.5f + spanMin);

    if (allowAAB) {
        box = AABox(spanMin, spanMax);
        boxV = box.volume();
    }

    if (allowSphere) {
        float radius(detMaxRadius(nVerts, positions, center));
        sphere = Sphere(center, radius);
        sphereV = sphere.volume();
    }

    if (allowCapsule) {
        float minRad, yUpper, yLower;
        std::tie(minRad, yUpper, yLower) = detCapsuleSpecs(nVerts, positions, center);
        float capsuleHeight(yUpper - yLower);
        glm::vec3 capsuleCenter(center.x, yLower + capsuleHeight * 0.5f, center.z);
        capsule = Capsule(capsuleCenter, minRad, capsuleHeight);
        capsuleV = capsule.volume();
    }

    if (allowSphere && sphereV <= boxV && sphereV <= capsuleV) {
        return Scene::addComponentAs<SphereBounderComponent, BounderComponent>(gameObject, weight, sphere);
    }
    else if (allowAAB && boxV <= sphereV && boxV <= capsuleV) {
        return Scene::addComponentAs<AABBounderComponent, BounderComponent>(gameObject, weight, box);
    }
    else {
        return Scene::addComponentAs<CapsuleBounderComponent, BounderComponent>(gameObject, weight, capsule);
    }
}