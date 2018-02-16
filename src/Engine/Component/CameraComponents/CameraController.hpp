/* Controls camera using mouse and keyboard */
#pragma once
#ifndef _CAMERA_CONTROLLER_HPP_
#define _CAMERA_CONTROLLER_HPP_

#include "Component/Component.hpp"
#include "CameraComponent.hpp"

class GameLogicSystem;

class CameraController : public Component {

    friend Scene;

    protected: // only scene can create component

        CameraController(CameraComponent & cc, float ls, float ms, int f, int b, int l, int r, int u, int d) :
            m_spatial(*cc.getGameObject()->getSpatial()),
            m_camera(cc),
            m_lookSpeed(ls),
            m_moveSpeed(ms),
            m_front(f),
            m_back(b),
            m_left(l),
            m_right(r),
            m_up(u),
            m_down(d)
        {}

    public:

        virtual SystemID systemID() const override { return SystemID::gameLogic; };

        void init();

        void update(float dt);

        void lookAround(float dt);
        void moveFront(float dt);
        void moveBack(float dt);
        void moveLeft(float dt);
        void moveRight(float dt);
        void moveUp(float dt);
        void moveDown(float dt);
        void strafeForward(float dt);
        void strafeBackward(float dt);

    private:

        SpatialComponent & m_spatial;
        CameraComponent & m_camera;
        float m_lookSpeed, m_moveSpeed;
        int m_front, m_back, m_left, m_right, m_up, m_down;

};

#endif
