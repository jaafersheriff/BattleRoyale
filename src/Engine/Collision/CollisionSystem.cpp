#include "CollisionSystem.hpp"

#include <unordered_set>
#include <unordered_map>



namespace {



struct Collision {

    Bounder * b1, * b2;
    fvec3 d1, d2;

    Collision() :
        b1(nullptr), b2(nullptr),
        d1(), d2()
    {}

    Collision(Bounder * b1, Bounder * b2, const fvec3 & d1, const fvec3 & d2) :
        b1(b1), b2(b2),
        d1(d1), d2(d2)
    {}

};



std::vector<Bounder *> f_bounders;
qhm::Map<WorldEntity *, std::vector<Bounder *>> f_entityBounders;
std::unordered_set<Bounder *> f_potentials;



fvec3 axisVec(int axis, bool sign) {
    static const fvec3 k_posAxisVecs[3]{ qcu::xAxis3<float>,  qcu::yAxis3<float>,  qcu::zAxis3<float> };
    static const fvec3 k_negAxisVecs[3]{-qcu::xAxis3<float>, -qcu::yAxis3<float>, -qcu::zAxis3<float> };

    return sign ? k_posAxisVecs[axis] : k_negAxisVecs[axis];
}

float detPointiness(nat n, const fvec3 * locs, const fvec3 & center) {
    std::unique_ptr<float[]> vals(new float[n]);
    nat m(0);
    for (nat i(0); i < n; ++i) {
        fvec3 p(qcu::abs(locs[i] - center));
        if (qcu::zero(p)) {
            continue;
        }
        vals[m++] = qcu::min(p) / qcu::max(p);
    }

    if (m == 0) {
        return 0.0f;
    }
    return qcu::pairwiseSum(m, vals.get()) / float(m);
}

float detMaxRadius(nat n, const fvec3 * locs, const fvec3 & center) {
    float maxR2(0.0f);
    for (nat i(0); i < n; ++i) {
        float r2(qcu::magnitude2(locs[i] - center));
        if (r2 > maxR2) maxR2 = r2;
    }
    return std::sqrt(maxR2);
}

// returns min radius, absolute z upper, and absolute z lower
fvec3 detCapsuleSpecs(nat n, const fvec3 * locs, const fvec3 & center) {
    float maxR2(0.0f);
    for (nat i(0); i < n; ++i) {
        float r2(qcu::magnitude2(fvec2(locs[i] - center)));
        if (r2 > maxR2) maxR2 = r2;
    }
    float r(std::sqrt(maxR2));

    float maxQz(-qcu::infinity<float>), minQz(qcu::infinity<float>);
    for (nat i(0); i < n; ++i) {
        float a(std::sqrt(maxR2 - qcu::magnitude2(locs[i].xy() - center.xy())));
        float qz(locs[i].z - qcu::sign(locs[i].z - center.z) * a);
        if (qz > maxQz) maxQz = qz;
        if (qz < minQz) minQz = qz;
    }

    return fvec3(r, maxQz, minQz);
}

bool collide(const AABox & box1, const AABox & box2, fvec3 * delta_) {
    if (!(qcu::conjunct(box1.min < box2.max) && qcu::conjunct(box1.max > box2.min))) {
        return false;
    }
    if (!delta_) {
        return true;
    }

    fvec3 delta1(box2.max - box1.min);
    fvec3 delta2(box2.min - box1.max);
    fvec3 delta(
        qcu::abs(delta1.x) <= qcu::abs(delta2.x) ? delta1.x : delta2.x,
        qcu::abs(delta1.y) <= qcu::abs(delta2.y) ? delta1.y : delta2.y,
        qcu::abs(delta1.z) <= qcu::abs(delta2.z) ? delta1.z : delta2.z
    );

    // preserve only minimum component
    if (qcu::abs(delta.x) < qcu::abs(delta.y)) {
        delta.y = 0.0f;
        if (qcu::abs(delta.x < qcu::abs(delta.z))) {
            delta.z = 0.0f;
        }
        else {
            delta.x = 0.0f;
        }
    }
    else {
        delta.x = 0.0f;
        if (qcu::abs(delta.y) < qcu::abs(delta.z)) {
            delta.z = 0.0f;
        }
        else {
            delta.y = 0.0f;
        }
    }

    *delta_ = delta;

    return true;
}

bool collide(const AABox & box, const Sphere & sphere, fvec3 * delta_) {
    fvec3 p(qcu::clamp(sphere.origin, box.min, box.max));    
    fvec3 dVec(p - sphere.origin);
    float d2(qcu::magnitude2(dVec));
    
    if (d2 >= sphere.radius * sphere.radius) {
        return false;
    }
    if (!delta_) {
        return true;
    }

    fvec3 delta(qcu::zAxis3<float>);
    // sphere center is outside box
    if (!qcu::zero(d2)) {
        float d(std::sqrt(d2));
        delta = (sphere.radius - d) * (dVec / d);
    }
    // sphere center is inside box
    else {
        // nearest corner
        p.x = sphere.origin.x - box.min.x <= box.max.x - sphere.origin.x ? box.min.x : box.max.x;
        p.y = sphere.origin.y - box.min.y <= box.max.y - sphere.origin.y ? box.min.y : box.max.y;
        p.z = sphere.origin.z - box.min.z <= box.max.z - sphere.origin.z ? box.min.z : box.max.z;

        delta = sphere.origin - p;
        fvec3 aDelta(qcu::abs(delta));
        fvec3 boxC(box.min + (box.max - box.min) * 0.5f);

        // preserve only minimum component and add radius
        if (aDelta.x < aDelta.y) {
            delta.y = 0.0f;
            if (aDelta.x < aDelta.z) {
                delta.z = 0.0f;
                delta.x += sphere.radius * qcu::sign(boxC.x - sphere.origin.x);
            }
            else {
                delta.x = 0.0f;
                delta.z += sphere.radius * qcu::sign(boxC.z - sphere.origin.z);
            }
        }
        else {
            delta.x = 0.0f;
            if (aDelta.y < aDelta.z) {
                delta.z = 0.0f;
                delta.y += sphere.radius * qcu::sign(boxC.y - sphere.origin.y);
            }
            else {
                delta.y = 0.0f;
                delta.z += sphere.radius * qcu::sign(boxC.z - sphere.origin.z);
            }
        }
    }

    *delta_ = delta;

    return true;
}

bool collide(const AABox & box, const Capsule & cap, fvec3 * delta_) {
    fvec3 boxP(qcu::clamp(cap.center, box.min, box.max));
    fvec3 lowP(cap.center); lowP.z -= cap.height * 0.5f;
    fvec3 highP(cap.center); highP.z += cap.height * 0.5f;
    fvec3 rodP(cap.center.xy(), qcu::clamp(boxP.z, lowP.z, highP.z));
    fvec3 dVec(boxP - rodP);
    float d2(qcu::magnitude2(dVec));
    
    if (d2 >= cap.radius * cap.radius) {
        return false;
    }
    if (!delta_) {
        return true;
    }

    fvec3 delta(qcu::zAxis3<float>);
    // rod core is outside box
    if (!qcu::zero(d2)) {
        float d(std::sqrt(d2));
        delta = (cap.radius - d) * (dVec / d);
    }
    // rod core is at least partially inside box
    else {
        fvec3 boxC(box.min + (box.max - box.min) * 0.5f);

        if (cap.center.z < boxC.z) {
            boxP.z = qcu::clamp(highP.z, box.min.z, box.max.z);
        }
        else {
            boxP.z = qcu::clamp(lowP.z, box.min.z, box.max.z);
        }

        // nearest corner
        fvec3 c(
            boxP.x - box.min.x <= box.max.x - boxP.x ? box.min.x : box.max.x,
            boxP.y - box.min.y <= box.max.y - boxP.y ? box.min.y : box.max.y,
            boxP.z - box.min.z <= box.max.z - boxP.z ? box.min.z : box.max.z
        );

        delta = boxP - c;
        fvec3 aDelta(qcu::abs(delta));

        // preserve only minimum component and add radius
        if (aDelta.x < aDelta.y) {
            delta.y = 0.0f;
            if (aDelta.x < aDelta.z) {
                delta.z = 0.0f;
                delta.x += cap.radius * qcu::sign(boxC.x - boxP.x);
            }
            else {
                delta.x = 0.0f;
                if (boxP.z - boxC.z > 0) {
                    delta.z = lowP.z - box.max.z - cap.radius;
                }
                else {
                    delta.z = highP.z - box.min.z + cap.radius;
                }
            }
        }
        else {
            delta.x = 0.0f;
            if (aDelta.y < aDelta.z) {
                delta.z = 0.0f;
                delta.y += cap.radius * qcu::sign(boxC.y - boxP.y);
            }
            else {
                delta.y = 0.0f;
                if (boxP.z - boxC.z > 0) {
                    delta.z = lowP.z - box.max.z - cap.radius;
                }
                else {
                    delta.z = highP.z - box.min.z + cap.radius;
                }
            }
        }
    }

    *delta_ = delta;

    return true;
}

bool collide(const Sphere & sphere1, const Sphere & sphere2, fvec3 * delta) {
    float combR(sphere1.radius + sphere2.radius);
    fvec3 dVec(sphere1.origin - sphere2.origin);
    float d2(qcu::magnitude2(dVec));
    
    if (d2 >= combR * combR) {
        return false;
    }
    if (!delta) {
        return true;
    }

    if (qcu::zero(d2)) {
        *delta = fvec3(0.0f, 0.0f, combR);
    }
    else {
        float d(std::sqrt(d2));
        *delta = (combR - d) * (dVec / d);
    }

    return true;
}

bool collide(const Sphere & sphere, const Capsule & cap, fvec3 * delta) {
    float combR(cap.radius + sphere.radius);
    fvec3 lowP(cap.center); lowP.z -= cap.height * 0.5f;
    fvec3 highP(cap.center); highP.z += cap.height * 0.5f;
    fvec3 rodP(cap.center.xy(), qcu::clamp(sphere.origin.z, lowP.z, highP.z));
    fvec3 dVec(sphere.origin - rodP);
    float d2(qcu::magnitude2(dVec));

    if (d2 >= combR * combR) {
        return false;
    }
    if (!delta) {
        return true;
    }

    if (qcu::zero(d2)) {
        if (sphere.origin.z >= cap.center.z) {
            *delta = fvec3(0.0f, 0.0f, highP.z - sphere.origin.z + combR);
        }
        else {
            *delta = fvec3(0.0f, 0.0f, lowP.z - sphere.origin.z - combR);
        }
    }
    else {
        float d(std::sqrt(d2));
        *delta = (combR - d) * (dVec / d);
    }

    return true;
}

bool collide(const Capsule & cap1, const Capsule & cap2, fvec3 * delta) {
    float combR(cap1.radius + cap2.radius);
    fvec3 lowP1(cap1.center); lowP1.z -= cap1.height * 0.5f;
    fvec3 highP1(cap1.center); highP1.z += cap1.height * 0.5f;
    fvec3 lowP2(cap2.center); lowP2.z -= cap2.height * 0.5f;
    fvec3 highP2(cap2.center); highP2.z += cap2.height * 0.5f;
    fvec3 rodP1(cap1.center.xy(), qcu::clamp(cap2.center.z, lowP1.z, highP1.z));
    fvec3 rodP2(cap2.center.xy(), qcu::clamp(rodP1.z, lowP2.z, highP2.z));
    fvec3 dVec(rodP1 - rodP2);
    float d2(qcu::magnitude2(dVec));

    if (d2 >= combR * combR) {
        return false;
    }
    if (!delta) {
        return true;
    }

    if (qcu::zero(d2)) {
        if (cap1.center.z >= cap2.center.z) {
            *delta = fvec3(0.0f, 0.0f, highP2.z - lowP1.z + combR);
        }
        else {
            *delta = fvec3(0.0f, 0.0f, lowP2.z - highP1.z - combR);
        }
    }
    else {
        float d(std::sqrt(d2));
        *delta = (combR - d) * (dVec / d);
    }

    return true;
}

bool collide(const AABBBounder & b1, const Bounder & b2, fvec3 * delta) {
    if (dynamic_cast<const AABBBounder *>(&b2)) 
        return collide(b1.transBox(), static_cast<const AABBBounder &>(b2).transBox(), delta);
    else if (dynamic_cast<const SphereBounder *>(&b2))
        return collide(b1.transBox(), static_cast<const SphereBounder &>(b2).transSphere(), delta);
    else if (dynamic_cast<const CapsuleBounder *>(&b2))
        return collide(b1.transBox(), static_cast<const CapsuleBounder &>(b2).transCapsule(), delta);
    return false;
}

bool collide(const SphereBounder & b1, const Bounder & b2, fvec3 * delta) {
    if (dynamic_cast<const AABBBounder *>(&b2)) {
        bool res(collide(static_cast<const AABBBounder &>(b2).transBox(), b1.transSphere(), delta));
        *delta *= -1.0f;
        return res;
    }
    else if (dynamic_cast<const SphereBounder *>(&b2))
        return collide(b1.transSphere(), static_cast<const SphereBounder &>(b2).transSphere(), delta);
    else if (dynamic_cast<const CapsuleBounder *>(&b2))
        return collide(b1.transSphere(), static_cast<const CapsuleBounder &>(b2).transCapsule(), delta);
    return false;
}

bool collide(const CapsuleBounder & b1, const Bounder & b2, fvec3 * delta) {
    if (dynamic_cast<const AABBBounder *>(&b2)) {
        bool res(collide(static_cast<const AABBBounder &>(b2).transBox(), b1.transCapsule(), delta));
        *delta *= -1.0f;
        return res;
    }
    else if (dynamic_cast<const SphereBounder *>(&b2)) {
        bool res(collide(static_cast<const SphereBounder &>(b2).transSphere(), b1.transCapsule(), delta));
        *delta *= -1.0f;
        return res;
    }
    else if (dynamic_cast<const CapsuleBounder *>(&b2))
        return collide(b1.transCapsule(), static_cast<const CapsuleBounder &>(b2).transCapsule(), delta);
    return false;
}

bool collide(const Bounder & b1, const Bounder & b2, fvec3 * delta) {
    if (dynamic_cast<const AABBBounder *>(&b1))
        return collide(static_cast<const AABBBounder &>(b1), b2, delta);
    else if (dynamic_cast<const SphereBounder *>(&b1))
        return collide(static_cast<const SphereBounder &>(b1), b2, delta);
    else if (dynamic_cast<const CapsuleBounder *>(&b1))
        return collide(static_cast<const CapsuleBounder &>(b1), b2, delta);
    return false;
}

bool collide(Bounder & b1, Bounder & b2, std::unordered_map<Bounder *, fvec3> * collisions) {
    fvec3 delta;
    if (!collide(b1, b2, &delta)) {
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



Bounder::Bounder(WorldEntity & entity, int weight) :
    m_entity(entity),
    m_weight(weight)
{}



AABBBounder::AABBBounder(WorldEntity & entity, int weight, const AABox & box) :
    Bounder(entity, weight),
    m_box(box),
    m_transBox(m_box)
{}

void AABBBounder::update() {
    // no rotation
    if (qcu::isIdentity(m_entity.rot())) {
        m_transBox.min = m_box.min * m_entity.scale() + m_entity.loc();
        m_transBox.max = m_box.max * m_entity.scale() + m_entity.loc();
    }
    // is rotation
    else {
        fvec3 corners[8]{
            fvec3(m_box.min.x, m_box.min.y, m_box.min.z),
            fvec3(m_box.max.x, m_box.min.y, m_box.min.z),
            fvec3(m_box.min.x, m_box.max.y, m_box.min.z),
            fvec3(m_box.max.x, m_box.max.y, m_box.min.z),
            fvec3(m_box.min.x, m_box.min.y, m_box.max.z),
            fvec3(m_box.max.x, m_box.min.y, m_box.max.z),
            fvec3(m_box.min.x, m_box.max.y, m_box.max.z),
            fvec3(m_box.max.x, m_box.max.y, m_box.max.z)
        };
        const qcu::fmat4 & entityMat(m_entity.absMat());
        for (int i(0); i < 8; ++i) {
            corners[i] = entityMat * fvec4(corners[i], 1.0f);
        }
        m_transBox.min = corners[0];
        m_transBox.max = corners[0];
        for (int i(1); i < 8; ++i) {
            m_transBox.min = qcu::min(m_transBox.min, corners[i]);
            m_transBox.max = qcu::max(m_transBox.max, corners[i]);
        }
    }
}

Intersect AABBBounder::intersect(const Ray & ray) const {

    struct T {

        float t;
        int axis;
        bool highLow;

        T(float t, int axis, bool highLow) : t(t), axis(axis), highLow(highLow) {}
        
        bool operator<(const T & other) const { return t < other.t; }        
        bool operator>(const T & other) const { return t > other.t; }
    
    };

    fvec3 invDir(1.0f / ray.dir);

    vec3<T> tsLow(
        T((m_transBox.min.x - ray.loc.x) * invDir.x, 0, false),
        T((m_transBox.min.y - ray.loc.y) * invDir.y, 1, false),
        T((m_transBox.min.z - ray.loc.z) * invDir.z, 2, false)
    );
    vec3<T> tsHigh(
        T((m_transBox.max.x - ray.loc.x) * invDir.x, 0, true),
        T((m_transBox.max.y - ray.loc.y) * invDir.y, 1, true),
        T((m_transBox.max.z - ray.loc.z) * invDir.z, 2, true)
    );

    T tMin(qcu::max(qcu::min(tsLow, tsHigh)));
    T tMax(qcu::min(qcu::max(tsLow, tsHigh)));

     // extra negation so that a NaN case returns no intersection
    if (tMax.t <= 0.0f || !(tMax.t >= tMin.t)) {
        return Intersect();
    }

    // exterior collision
    if (tMin.t >= 0.0f) {
        fvec3 loc(ray.loc + tMin.t * ray.dir);
        fvec3 norm(axisVec(tMin.axis, tMin.highLow));

        return Intersect(*this, tMin.t, loc, norm, true);
    }
    // interior collision
    else {
        fvec3 loc(ray.loc + tMax.t * ray.dir);
        fvec3 norm(axisVec(tMax.axis, tMax.highLow));

        return Intersect(*this, tMax.t, loc, norm, false);
    }
}



SphereBounder::SphereBounder(WorldEntity & entity, int weight, const Sphere & sphere) :
    Bounder(entity, weight),
    m_sphere(sphere),
    m_transSphere(m_sphere)
{}

void SphereBounder::update() {
    m_transSphere.origin = m_entity.absMat() * fvec4(m_sphere.origin, 1.0f);
    m_transSphere.radius = qcu::max(static_cast<const WorldTransformable &>(m_entity).scale()) * m_sphere.radius;
}

Intersect SphereBounder::intersect(const Ray & ray) const {    
    fvec3 C(m_transSphere.origin - ray.loc);
    float rad2(m_transSphere.radius * m_transSphere.radius);
    bool face(qcu::magnitude2(C) - rad2 >= 0.0f);

    float p(qcu::dot(ray.dir, C));

    if (face && p < 0.0f) {
        return Intersect();
    }

    fvec3 P(p * ray.dir);
    float d2(qcu::magnitude2(P - C));

    if (d2 >= rad2) {
        return Intersect();
    }

    float h(face ? std::sqrt(rad2 - d2) : -std::sqrt(rad2 - d2));
    fvec3 I(P - h * ray.dir);

    return Intersect(*this, p - h, ray.loc + I, (I - C) / m_transSphere.radius, face);
}



CapsuleBounder::CapsuleBounder(WorldEntity & entity, int weight, const Capsule & capsule) :
    Bounder(entity, weight),
    m_capsule(capsule),
    m_transCapsule(m_capsule)
{}

void CapsuleBounder::update() {
    m_transCapsule.center = m_entity.absMat() * fvec4(m_capsule.center, 1.0f);
    const fvec3 & scale(static_cast<const WorldTransformable &>(m_entity).scale());
    m_transCapsule.radius = qcu::max(scale.xy()) * m_capsule.radius;
    m_transCapsule.height = qcu::max(0.0f, scale.z * (m_capsule.height + 2.0f * m_capsule.radius) - 2.0f * m_transCapsule.radius);
}

Intersect CapsuleBounder::intersect(const Ray & ray) const {
    return Intersect();
}



void add(WorldEntity & entity, int weight) {
    fspan3 span(entity.mesh().detSpan());
    AABox box(span);
    float boxV(box.volume());

    fvec3 center((span.max - span.min) * 0.5f + span.min);
    float radius(detMaxRadius(entity.mesh().nVerts, entity.mesh().getLocations(), center));
    Sphere sphere(center, radius);
    float sphereV(sphere.volume());

    fvec3 capsuleSpecs(detCapsuleSpecs(entity.mesh().nVerts, entity.mesh().getLocations(), center));
    float capsuleHeight(capsuleSpecs.y - capsuleSpecs.z);
    fvec3 capsuleCenter(center.x, center.y, capsuleSpecs.z + capsuleHeight * 0.5f);
    Capsule capsule(capsuleCenter, capsuleSpecs.x, capsuleHeight);
    float capsuleV(capsule.volume());

    Bounder * bounder;
    if (boxV <= sphereV) {
        if (boxV <= capsuleV) {
            bounder = qcu::Depot<Bounder>::add(new AABBBounder(entity, weight, box));
        }
        else {
            bounder = qcu::Depot<Bounder>::add(new CapsuleBounder(entity, weight, capsule));
        }
    }
    else {
        if (sphereV <= capsuleV) {
            bounder = qcu::Depot<Bounder>::add(new SphereBounder(entity, weight, sphere));
        }
        else {
            bounder = qcu::Depot<Bounder>::add(new CapsuleBounder(entity, weight, capsule));
        }
    }

    f_bounders.push_back(bounder);
    f_entityBounders[&entity].push_back(bounder);
    f_potentials.insert(bounder);
    BounderRenderer::get().add(*bounder);
    bounder->update();
}

void addPotential(WorldEntity & entity) {
    if (!f_entityBounders.count(&entity)) {
        return;
    }
    for (auto & bounder : f_entityBounders.at(&entity)) {
        f_potentials.insert(bounder);
    }
}

void update() { 
    static std::unordered_map<Bounder *, fvec3> s_collisions;
    static std::unordered_set<Bounder *> s_checked;

    for (auto & potential : f_potentials) {
        potential->update();
        potential->m_isCollision = false;
    }
    for (auto & potential : f_potentials) {
        s_checked.insert(potential);
        for (auto & bounder : f_bounders) {
            if (s_checked.count(bounder) || &potential->entity() == &bounder->entity()) {
                continue;
            }
            if (collide(*potential, *bounder, &s_collisions)) {

            }
        }
    }
    f_potentials.clear();
    s_checked.clear();
    
    for (auto & [bounder, delta] : s_collisions) {
        if (!qcu::zero(delta)) {
            bounder->entity().setLocation(bounder->entity().loc() + delta * config::collision::offsetFactor);
            bounder->update();
            bounder->m_isCollision = true;
        }
    }
    s_collisions.clear();
}

Intersect pick(const Ray & ray) {
    Intersect inter;
    for (const auto & bounder : f_bounders) {
        Intersect potential(bounder->intersect(ray));
        if (potential.dist < inter.dist) {
            inter = potential;
        }
    }
    return inter;
}