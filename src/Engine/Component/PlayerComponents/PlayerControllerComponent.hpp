#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"



class GameLogicSystem;
class CameraComponent;
class SpatialComponent;
class NewtonianComponent;
class GroundComponent;



class PlayerControllerComponent : public Component {

    friend Scene;
    friend GameLogicSystem;

    protected: // only scene or friends can create component

    PlayerControllerComponent(GameObject & gameObject, float lookSpeed, float moveSpeed, float jumpSpeed, float sprintSpeed);

    public:

    PlayerControllerComponent(PlayerControllerComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual SystemID systemID() const override { return SystemID::gameLogic; };

    virtual void update(float dt) override;

    void setEnabled(bool enabled);

    bool enabled() const { return m_enabled; }

    private:

    SpatialComponent * m_spatial;
    NewtonianComponent * m_newtonian;
    GroundComponent * m_ground;
    CameraComponent * m_camera;
    float m_lookSpeed;
    float m_moveSpeed;
    float m_jumpSpeed;
    float m_sprintSpeed;
    bool m_jumping;
    bool m_enabled; // TODO: make a way to enable/disable any object or component

};