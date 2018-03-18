#pragma once

#include <iostream>

#include "glm/glm.hpp"

#include "Component/Component.hpp"

class PathfindingSystem;

class PathfindingComponent : public Component {

    friend Scene;
    
    protected: // only scene or friends can create component

    PathfindingComponent(GameObject & gameObject, const GameObject & player, float ms, bool wander);

    public:

    PathfindingComponent(PathfindingComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual void update(float) override;

    // TODO : just add enable/disable options for all components?
    void setMoveSpeed(float f) { this->m_moveSpeed = f; }

    private:

    SpatialComponent * m_spatial;
    const GameObject & m_player;
    float m_moveSpeed;
    bool m_wander;
    glm::vec3 m_wanderCurrent;
    float m_wanderCurrentWeight;
    float m_wanderWeight;
};