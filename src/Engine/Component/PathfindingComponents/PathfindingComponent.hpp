#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"
#include <iostream>

class PathfindingSystem;

class PathfindingComponent : public Component {

    friend Scene;
    
    protected: // only scene or friends can create component

    PathfindingComponent(GameObject & player, float ms);

    virtual void init(GameObject & go) override;

    public:

    virtual SystemID systemID() const override { return SystemID::pathfinding; };

    virtual void update(float) override;

    private:

    SpatialComponent * m_spatial;
    GameObject * m_player;
    float m_moveSpeed;
};