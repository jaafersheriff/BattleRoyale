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

        virtual void update(float dt) override;

        void setOrientation(float theta, float phi);

        void setEnabled(bool enabled);

        float theta() const { return m_theta; }
        float phi() const { return m_phi; }

        bool enabled() const { return m_enabled; }

    private:

        void updateSpatialOrientation();

    private:

        SpatialComponent * m_spatial;
        float m_theta, m_phi;
        float m_lookSpeed;
        float m_minMoveSpeed, m_maxMoveSpeed;
        float m_moveSpeed;
        bool m_enabled; // TODO: way for any component / game object to be enabled/disabled

};
