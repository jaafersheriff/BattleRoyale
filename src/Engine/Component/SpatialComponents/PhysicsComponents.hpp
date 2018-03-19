#pragma once

#include "glm/glm.hpp"

#include "Component/Component.hpp"



class Scene;
class SpatialSystem;
class SpatialComponent;



// has a velocity and can undergo classical mechanics operations
class NewtonianComponent : public Component {

    friend Scene;

  protected: // only scene or friends can create component

    NewtonianComponent(GameObject & gameObject, bool isBouncy);

  public:

    NewtonianComponent(NewtonianComponent && other) = default;

    virtual ~NewtonianComponent() = default;

  protected:

    virtual void init() override;

  public:

    virtual void update(float dt) override;

    void accelerate(const glm::vec3 & acceleration);

    void addVelocity(const glm::vec3 & velocity);

    void setVelocity(const glm::vec3 & velocity);

    // dir should be normalized
    void removeAllVelocityAgainstDir(const glm::vec3 & dir);
    // dir should be normalized
    void removeSomeVelocityAgainstDir(const glm::vec3 & dir, float amount);

    const glm::vec3 & velocity() const { return m_velocity; }

  protected:

    SpatialComponent * m_spatial;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    bool m_isBouncy;


};



// accelerates the object by the given amount
class AcceleratorComponent : public Component {

    friend Scene;

  protected: // only scene or friends can create component

    AcceleratorComponent(GameObject & gameObject, const glm::vec3 & acceleration);

  public:

    AcceleratorComponent(AcceleratorComponent && other) = default;

    virtual ~AcceleratorComponent() = default;

  protected:

    virtual void init() override;

  public:

    virtual void update(float dt) override;

  protected:

    NewtonianComponent * m_newtonian;
    glm::vec3 m_acceleration;

};



// accelerates the object by gravity
// *** HAS TO BE ADDED TO SCENE AS ACCELERATORCOMPONENT ***
class GravityComponent : public AcceleratorComponent {

    friend Scene;

  protected: // only scene or friends can create component

    GravityComponent(GameObject & gameObject);

  public:

    GravityComponent(GravityComponent && other) = default;

    virtual ~GravityComponent() = default;

    virtual void update(float dt) override;

};
