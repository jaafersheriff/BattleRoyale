#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include "Component/Component.hpp"



class Scene;



// keeps track of if the object is "on the ground" and what slope the "ground" is
class GroundComponent : public Component {

    friend Scene;

  public:

    static constexpr float k_defCriticalAngle = glm::pi<float>() * 0.25f; // 45 degrees

  protected: // only scene or friends can create component

    GroundComponent(float criticalAngle = k_defCriticalAngle);

  public:

    GroundComponent(GroundComponent && other) = default;

    virtual ~GroundComponent() = default;

  protected:

    virtual void init(GameObject & go) override;

  public:

    virtual SystemID systemID() const override { return SystemID::postCollision; };

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