#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"



class GameLogicSystem;
class CameraComponent;
class SpatialComponent;
class NewtonianComponent;



class PlayerControllerComponent : public Component {

    friend Scene;
    friend GameLogicSystem;

    protected: // only scene or friends can create component

    PlayerControllerComponent(float lookSpeed, float moveSpeed);

    virtual void init(GameObject & gameObject) override;

    public:

    virtual SystemID systemID() const override { return SystemID::gameLogic; };

    virtual void update(float dt) override;

    void setEnabled(bool enabled);

    bool enabled() const { return m_enabled; }

    private:

    SpatialComponent * m_spatial;
    NewtonianComponent * m_newtonian;
    CameraComponent * m_camera;
    float m_lookSpeed, m_moveSpeed;
    bool m_enabled; // TODO: make a way to enable/disable any object or component

};