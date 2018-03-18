#pragma once

#include "glm/glm.hpp"

#include "Component/Component.hpp"



class Scene;



// keeps track of if the object is "on the ground" and what slope the "ground" is
class GroundComponent : public Component {

    friend Scene;

  public:

    static const float k_defCriticalAngle;

  protected: // only scene or friends can create component

    GroundComponent(GameObject & gameObject, float criticalAngle = k_defCriticalAngle);

  public:

    GroundComponent(GroundComponent && other) = default;

    virtual ~GroundComponent() = default;

  protected:

    virtual void init() override;

  public:

    virtual void update(float dt) override;

    bool onGround() const;

    const glm::vec3 & groundNorm() const { return m_groundNorm; }

    void setCriticalAngle(float criticalAngle);

    float cosCriticalAngle() const { return m_cosCriticalAngle; }

  protected:

    float m_cosCriticalAngle; // cosine of most severe angle that can still be considered "ground"
    glm::vec3 m_groundNorm;
    glm::vec3 m_potentialGroundNorm;

};