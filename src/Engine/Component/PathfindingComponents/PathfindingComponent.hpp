#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"
#include <iostream>

class PathfindingSystem;

class PathfindingComponent : public Component {

    friend Scene;
    
    protected: // only scene or friends can create component

    PathfindingComponent(GameObject & gameObject, GameObject & player, float ms);

    public:

    PathfindingComponent(PathfindingComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual SystemID systemID() const override { return SystemID::pathfinding; };

    virtual void update(float) override;

    // TODO : just add enable/disable options for all components?
    void setMoveSpeed(float f) { this->m_moveSpeed = f; }

    private:

    SpatialComponent * m_spatial;
    GameObject * m_player;
    float m_moveSpeed;
};