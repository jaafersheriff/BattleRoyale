#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include "Component/Component.hpp"



class Scene;
class SpatialSystem;
class SpatialComponent;



// has a velocity and can undergo classical mechanics operations
class NewtonianComponent : public Component {

    friend Scene;

  public:

    virtual SystemID systemID() const override { return SystemID::spatial; };

  protected: // only scene or friends can create component

    NewtonianComponent(float maxSpeed);

  public:

    virtual ~NewtonianComponent() = default;

  protected:

    virtual void init(GameObject & go) override;

  public:

    virtual void update(float dt) override;

    void accelerate(const glm::vec3 & acceleration);

    void addVelocity(const glm::vec3 & velocity);

    // dir should be normalized
    void removeAllVelocityAgainstDir(const glm::vec3 & dir);
    // dir should be normalized
    void removeSomeVelocityAgainstDir(const glm::vec3 & dir, float amount);

    const glm::vec3 & velocity() const { return m_velocity; }

  protected:

    SpatialComponent * m_spatial;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    float m_maxSpeed;


};



// accelerates the object by the given amount
class AcceleratorComponent : public Component {

    friend Scene;

  public:

    virtual SystemID systemID() const override { return SystemID::spatial; };

  protected: // only scene or friends can create component

    AcceleratorComponent(const glm::vec3 & acceleration);

  public:

    virtual ~AcceleratorComponent() = default;

  protected:

    virtual void init(GameObject & go) override;

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

    GravityComponent();

  public:

    virtual ~GravityComponent() = default;

    virtual void update(float dt) override;

};
