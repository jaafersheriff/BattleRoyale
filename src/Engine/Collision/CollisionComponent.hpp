#pragma once



#include "Component/Component.hpp"
#include "Util/Geometry.hpp"



// forward declaration
class Bounder;



// An intersection represents the intersection point of a bounder and a ray.
struct Intersect {    

    const Bounder * bounder; // the intersected bounder, or null if no intersection
    float dist; // the distance from the ray origin to the intersection point
    glm::vec3 loc; // the intersection point on the surface of the bounder
    glm::vec3 norm; // the normal of the surface of the bounder at the point of intersection
    bool face; // true if the ray hit the outside surface, false if inside surface

    Intersect::Intersect() :
        bounder(nullptr),
        dist(std::numeric_limits<float>::infinity()),
        loc(),
        norm(),
        face(true)
    {}

    Intersect::Intersect(
        const Bounder & bounder,
        float dist,
        const glm::vec3 & loc,
        const glm::vec3 & norm,
        bool face
    ) :
        bounder(&bounder),
        dist(dist),
        loc(loc),
        norm(norm),
        face(face)
    {}

};



// Represents a bounding surface around an entity
class Bounder : public Component {

    protected:

    WorldEntity & m_entity;
    int m_weight;

    public:

    bool m_isCollision = false;

    protected:

    Bounder(WorldEntity & entity, int weight);

    public:

    virtual ~Bounder() override = default;

    virtual void update() = 0;

    virtual Intersect intersect(const Ray & ray) const = 0;

    WorldEntity & entity() const { return m_entity; }
    int weight() const { return m_weight; }
    bool isCollision() const { return m_isCollision; }

};



class AABBounder : public Bounder {

    private:

    const AABox m_box;
    AABox m_transBox;

    public:

    AABBounder(WorldEntity & entity, int weight, const AABox & box);

    virtual void update() override;

    virtual Intersect intersect(const Ray & ray) const override;

    const AABox & box() const { return m_box; }
    const AABox & transBox() const { return m_transBox; }

};



class SphereBounder : public Bounder {

    private:

    const Sphere m_sphere;
    Sphere m_transSphere;

    public:

    SphereBounder(WorldEntity & entity, int weight, const Sphere & sphere);

    virtual void update() override;

    virtual Intersect intersect(const Ray & ray) const override;

    const Sphere & sphere() const { return m_sphere; }
    const Sphere & transSphere() const { return m_transSphere; }

};



class CapsuleBounder : public Bounder {

    private:

    const Capsule m_capsule;
    Capsule m_transCapsule;

    public:

    CapsuleBounder(WorldEntity & entity, int weight, const Capsule & capsule);

    virtual void update() override;

    virtual Intersect intersect(const Ray & ray) const override;

    const Capsule & capsule() const { return m_capsule; }
    const Capsule & transCapsule() const { return m_transCapsule; }

};