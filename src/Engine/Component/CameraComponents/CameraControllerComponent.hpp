/* Controls camera using mouse and keyboard */
#pragma once



#include "Component/Component.hpp"



class GameLogicSystem;
class CameraComponent;
class Scene;



class CameraControllerComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

        CameraControllerComponent(float ls, float ms);

        virtual void init(GameObject & gameObject) override;

    public:

        virtual SystemID systemID() const override { return SystemID::gameLogic; };

        virtual void update(float dt) override;

        void setEnabled(bool enabled);

        bool enabled() const { return m_enabled; }

    private:

        SpatialComponent * m_spatial;
        CameraComponent * m_camera;
        float m_lookSpeed, m_moveSpeed;
        bool m_enabled; // TODO: way for any component / game object to be enabled/disabled

};
