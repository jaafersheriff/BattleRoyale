#pragma once
#ifndef _MAPEXPLORE_COMPONENT
#define _MAPEXPLORE_COMPONENT

#include "Component/Component.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
//#include "Component/PathfindingComponents/PathfindingComponent.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/string_cast.hpp"

#include <iostream>
#include <queue>

#include "Loader/Loader.hpp"

//#include "System/MapExploreSystem.hpp"
//#include "System/PathfindingSystem.hpp"

//#include "Scene/Scene.hpp"
#include "Util/Util.hpp"



class MapExploreSystem;


class MapExploreComponent : public Component {

    friend Scene;
    
    protected: // only scene or friends can create component

    MapExploreComponent(GameObject & gameObject, GameObject & player, float ms);

    public:

    MapExploreComponent(MapExploreComponent && other) = default;

    // 45 degrees
    static constexpr float k_defCriticalAngle = glm::pi<float>() * 0.25f;

    protected:

    virtual void init() override;

    //void aStarSearch(detail::vecvectorMap &graph, glm::vec3 start, glm::vec3 end, detail::vecvecMap &cameFrom);//, detail::vecdoubleMap &cost);
    //Vector<glm::vec3> reconstructPath(glm::vec3 start, glm::vec3 end, detail::vecvecMap &cameFrom);
    //void readInGraph(String fileName); 
    //void print_queue(std::queue<glm::vec3> q);
    //void drawCup(glm::vec3 position);
    bool findInVisited(glm::vec3 vec, float stepSize);
    std::string vectorToString(Vector<glm::vec3> vec);
    glm::vec3 closestPos(glm::vec3 vec);
    void drawCup(glm::vec3 position);

    // cosine of most severe angle that can still be considered "ground"
    float m_cosCriticalAngle;

    public:

    virtual SystemID systemID() const override { return SystemID::mapexplore; };

    virtual void update(float) override;

    // TODO : just add enable/disable options for all components?
    void setMoveSpeed(float f) { this->m_moveSpeed = f; }

    private:

    //typedef std::unordered_map<glm::vec3, glm::vec3, detail::vecHash, detail::customVecCompare> vecvecMap;
    //typedef std::unordered_map<glm::vec3, double, detail::vecHash, detail::customVecCompare> vecdoubleMap;

    SpatialComponent * m_spatial;
    GameObject * m_player;
    float m_moveSpeed;

    glm::vec3 m_groundNorm;
    glm::vec3 m_potentialGroundNorm;

    glm::vec3 curPos;
    glm::vec3 searchFromPos;
    bool nonGroundCollision;
    bool writeOut;
    bool updatePath;
    int pathCount;

    detail::vecvecMap cameFrom;
    Vector<glm::vec3> path;
    std::vector<glm::vec3>::iterator pathIT;

    int slowTime;
    int dirIndex;
    int yIndex; 
    int *checkedDirections;

    glm::vec3 prevMove;
    std::queue<glm::vec3> pos_queue;
    std::unordered_set<glm::vec3, detail::vecHash, detail::customVecCompare> visitedSet;
    //Vector<glm::vec3> visitedSet;
    detail::vecvectorMap graph;
    Vector<glm::vec3> validNeighbors;

    std::unordered_map<glm::vec3, Node, detail::vecHash, detail::customVecCompare> vecToNode;

    bool m_wander;
    glm::vec3 m_wanderCurrent;
    float m_wanderCurrentWeight;
    float m_wanderWeight;
};

#endif