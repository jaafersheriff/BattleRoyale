#pragma once



#include "Component/Component.hpp"
#include "Util/Geometry.hpp"



// forward declaration
class CollisionSystem;
class BounderComponent;
class Mesh;



// Represents a bounding surface around an entity
class BounderComponent : public Component {

    friend CollisionSystem;

    public:

    using SystemClass = CollisionSystem;

    protected:

    int m_weight;
    bool m_isCollision; // was there a collision this tick

    public:

    protected:

    BounderComponent(int weight);

    public:

    virtual ~BounderComponent() override = default;

    virtual void update() = 0;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const = 0;

    virtual Intersect intersect(const Ray & ray) const = 0;

    int weight() const { return m_weight; }
    bool isCollision() const { return m_isCollision; }

};



class AABBounderComponent : public BounderComponent {

    private:

    const AABox m_box;
    AABox m_transBox;

    public:

    AABBounderComponent(int weight, const AABox & box);

    virtual void update() override;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const override;

    virtual Intersect intersect(const Ray & ray) const override;

    const AABox & box() const { return m_box; }
    const AABox & transBox() const { return m_transBox; }

};



class SphereBounderComponent : public BounderComponent {

    private:

    const Sphere m_sphere;
    Sphere m_transSphere;

    public:

    SphereBounderComponent(int weight, const Sphere & sphere);

    virtual void update() override;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const override;

    virtual Intersect intersect(const Ray & ray) const override;

    const Sphere & sphere() const { return m_sphere; }
    const Sphere & transSphere() const { return m_transSphere; }

};



class CapsuleBounderComponent : public BounderComponent {

    private:

    const Capsule m_capsule;
    Capsule m_transCapsule;

    public:

    CapsuleBounderComponent(int weight, const Capsule & capsule);

    virtual void update() override;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const override;

    virtual Intersect intersect(const Ray & ray) const override;

    const Capsule & capsule() const { return m_capsule; }
    const Capsule & transCapsule() const { return m_transCapsule; }

};