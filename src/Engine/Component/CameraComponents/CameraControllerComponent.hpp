/* Controls camera using mouse and keyboard */
#pragma once



#include "Component/Component.hpp"



class GameSystem;
class CameraComponent;
class Scene;



class CameraControllerComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

        CameraControllerComponent(GameObject & gameObject, float lookSpeed, float minMoveSpeed, float maxMoveSpeed);

    public:

        CameraControllerComponent(CameraControllerComponent && other) = default;

    protected:

        virtual void init() override;

    public:

        virtual SystemID systemID() const override { return SystemID::game; };

        virtual void update(float dt) override;

        void setEnabled(bool enabled);

        bool enabled() const { return m_enabled; }

    private:

        SpatialComponent * m_spatial;
        CameraComponent * m_camera;
        float m_lookSpeed;
        float m_minMoveSpeed, m_maxMoveSpeed;
        float m_moveSpeed;
        bool m_enabled; // TODO: way for any component / game object to be enabled/disabled

};
