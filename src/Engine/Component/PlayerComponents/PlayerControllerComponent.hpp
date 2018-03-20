#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"



class GameSystem;
class CameraComponent;
class SpatialComponent;
class NewtonianComponent;
class GroundComponent;



class PlayerControllerComponent : public Component {

    friend Scene;
    friend GameSystem;

    protected: // only scene or friends can create component

    PlayerControllerComponent(GameObject & gameObject, float lookSpeed, float moveSpeed, float jumpSpeed, float sprintSpeed);

    public:

    PlayerControllerComponent(PlayerControllerComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual void update(float dt) override;

    void setOrientation(float theta, float phi);

    void setEnabled(bool enabled);

    float theta() const { return m_theta; }
    float phi() const { return m_phi; }

    bool enabled() const { return m_enabled; }

    private:

    void updateSpatialOrientation();

    private:

    SpatialComponent * m_bodySpatial;
    SpatialComponent * m_headSpatial;
    NewtonianComponent * m_newtonian;
    GroundComponent * m_ground;
    float m_theta, m_phi;
    float m_lookSpeed;
    float m_moveSpeed;
    float m_jumpSpeed;
    float m_sprintSpeed;
    bool m_jumping;
    bool m_enabled; // TODO: make a way to enable/disable any object or component

};