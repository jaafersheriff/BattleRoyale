#pragma once



#include "Component/Component.hpp"
#include "Util/Geometry.hpp"



// forward declaration
class CollisionSystem;
class BounderComponent;
class BounderShader;
class Mesh;



// Represents a bounding surface around an entity
class BounderComponent : public Component {

    friend Scene;
    friend CollisionSystem;

    protected: // only scene or friends can create component

    BounderComponent(GameObject & gameObject, unsigned int weight);

    public:

    BounderComponent(BounderComponent && other) = default;

    virtual ~BounderComponent() override = default;

    protected:

    virtual void init() override;

    public:

    virtual void update(float dt) = 0;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const = 0;

    virtual Intersect intersect(const Ray & ray) const = 0;

    virtual AABox enclosingAABox() const = 0;
    virtual Sphere enclosingSphere() const = 0;

    virtual glm::vec3 center() const = 0;
    virtual glm::vec3 prevCenter() const = 0;

    // is the difference between the preset and previous bounders too great for standard collision
    virtual bool isCritical() const = 0;

    unsigned int weight() const { return m_weight; }

    bool isChange() const { return m_isChange; }

    protected:

    SpatialComponent * m_spatial;
    unsigned int m_weight;
    bool m_isChange;

};



// *** HAS TO BE ADDED TO SCENE AS BOUNDERCOMPONENT ***
class AABBounderComponent : public BounderComponent {
    
    friend Scene;
    friend CollisionSystem;

    public:

    static AABox transformAABox(const AABox & box, const glm::mat4 & transMat);
    static AABox transformAABox(const AABox & box, const glm::vec3 & position, const glm::vec3 & scale);

    protected: // only scene or friends can create component

    AABBounderComponent(GameObject & gameObject, unsigned int weight, const AABox & box);

    public:

    AABBounderComponent(AABBounderComponent && other) = default;

    virtual void update(float dt) override;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const override;

    virtual Intersect intersect(const Ray & ray) const override;
    
    virtual AABox enclosingAABox() const override;
    virtual Sphere enclosingSphere() const override;

    virtual glm::vec3 center() const override { return m_transBox.center(); }
    virtual glm::vec3 prevCenter() const override { return m_prevTransBox.center(); }

    virtual bool isCritical() const override;

    const AABox & box() const { return m_box; }
    const AABox & transBox() const { return m_transBox; }
    const AABox & prevTransBox() const { return m_prevTransBox; }

    private:

    const AABox m_box;
    AABox m_transBox;
    AABox m_prevTransBox;

};



// *** HAS TO BE ADDED TO SCENE AS BOUNDERCOMPONENT ***
class SphereBounderComponent : public BounderComponent {
    
    friend Scene;
    friend CollisionSystem;

    public:

    static Sphere transformSphere(const Sphere & sphere, const glm::mat4 & transMat, const glm::vec3 & scale);

    protected: // only scene or friends can create component

    SphereBounderComponent(GameObject & gameObject, unsigned int weight, const Sphere & sphere);

    public:

    SphereBounderComponent(SphereBounderComponent && other) = default;

    virtual void update(float dt) override;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const override;

    virtual Intersect intersect(const Ray & ray) const override;
    
    virtual AABox enclosingAABox() const override;
    virtual Sphere enclosingSphere() const override;

    virtual glm::vec3 center() const override { return m_transSphere.origin; }
    virtual glm::vec3 prevCenter() const override { return m_prevTransSphere.origin; }

    virtual bool isCritical() const override;

    const Sphere & sphere() const { return m_sphere; }
    const Sphere & transSphere() const { return m_transSphere; }
    const Sphere & prevTransSphere() const { return m_prevTransSphere; }

    private:

    const Sphere m_sphere;
    Sphere m_transSphere;
    Sphere m_prevTransSphere;

};



// *** HAS TO BE ADDED TO SCENE AS BOUNDERCOMPONENT ***
class CapsuleBounderComponent : public BounderComponent {
    
    friend Scene;
    friend CollisionSystem;

    public:

    static Capsule transformCapsule(const Capsule & capsule, const glm::mat4 & transMat, const glm::vec3 & scale);

    protected: // only scene or friends can create component

    CapsuleBounderComponent(GameObject & gameObject, unsigned int weight, const Capsule & capsule);

    public:

    CapsuleBounderComponent(CapsuleBounderComponent && other) = default;

    virtual void update(float dt) override;

    virtual bool collide(const BounderComponent & o, glm::vec3 * delta) const override;

    virtual Intersect intersect(const Ray & ray) const override;
    
    virtual AABox enclosingAABox() const override;
    virtual Sphere enclosingSphere() const override;

    virtual glm::vec3 center() const override { return m_transCapsule.center; }
    virtual glm::vec3 prevCenter() const override { return m_prevTransCapsule.center; }

    virtual bool isCritical() const override;

    const Capsule & capsule() const { return m_capsule; }
    const Capsule & transCapsule() const { return m_transCapsule; }
    const Capsule & prevTransCapsule() const { return m_prevTransCapsule; }

    private:

    const Capsule m_capsule;
    Capsule m_transCapsule;
    Capsule m_prevTransCapsule;

};