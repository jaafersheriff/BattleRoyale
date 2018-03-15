#pragma once
#ifndef _PATHFINDING_COMPONENT
#define _PATHFINDING_COMPONENT

#include "Component/Component.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>
#include <queue>

#include "Loader/Loader.hpp"

//#include "System/PathfindingSystem.hpp"

#include "Scene/Scene.hpp"
#include "Util/Util.hpp"

struct Node {
    glm::vec3 position;
    Vector<glm::vec3> neighbors;

    Node () :
        position(glm::vec3(0.0)),
        neighbors(Vector<glm::vec3>())
    {}

    Node(glm::vec3 pos, Vector<glm::vec3> neighbors) :
        position(pos),
        neighbors(neighbors)
    {}
};

namespace detail {

    struct vecHash
    {
      size_t operator()(const glm::vec3 &v) const {
        size_t h1 = std::hash<int>()(round(v.x));
        size_t h2 = std::hash<int>()(round(v.y));
        size_t h3 = std::hash<int>()(round(v.z));
        return (h1 ^ (h2 << 1)) ^ h3;
      }
    };

    struct nodeHash
    {
        size_t operator()(const Node &n) const {
            size_t h1 = std::hash<int>()(round(n.position.x));
            size_t h2 = std::hash<int>()(round(n.position.y));
            size_t h3 = std::hash<int>()(round(n.position.z));
            return (h1 ^ (h2 << 1)) ^ h3;
        }
    };


    struct customVecCompare {

        bool operator()(const glm::vec3& lhs, const glm::vec3& rhs) const
        {
            float radius = 1.f;

            if (glm::distance2(lhs, rhs) > radius * radius) {
                return false;
            }
            return true;
        }
    };

    typedef std::unordered_map<glm::vec3, glm::vec3, vecHash, customVecCompare> vecvecMap;
    typedef std::unordered_map<glm::vec3, double, vecHash, customVecCompare> vecdoubleMap;
    typedef std::unordered_map<glm::vec3, Vector<glm::vec3>, vecHash, customVecCompare> vecvectorMap;

}


class PathfindingSystem;


class PathfindingComponent : public Component {

    friend Scene;
    
    protected: // only scene or friends can create component

    PathfindingComponent(GameObject & gameObject, GameObject & player, float ms);
    PathfindingComponent(GameObject & gameObject, GameObject & player, float ms, bool wander);

    public:

    PathfindingComponent(PathfindingComponent && other) = default;

    // 45 degrees
    static constexpr float k_defCriticalAngle = glm::pi<float>() * 0.25f;

    protected:

    virtual void init() override;

    void aStarSearch(detail::vecvectorMap &graph, glm::vec3 start, glm::vec3 end, detail::vecvecMap &cameFrom);//, detail::vecdoubleMap &cost);
    Vector<glm::vec3> reconstructPath(glm::vec3 start, glm::vec3 end, detail::vecvecMap &cameFrom);
    void readInGraph(String fileName); 
    void print_queue(std::queue<glm::vec3> q);
    void drawCup(glm::vec3 position);
    bool findInVisited(glm::vec3 vec, float stepSize);
    std::string vectorToString(Vector<glm::vec3> vec);
    glm::vec3 closestPos(glm::vec3 vec);

    // cosine of most severe angle that can still be considered "ground"
    float m_cosCriticalAngle;

    public:

    virtual SystemID systemID() const override { return SystemID::pathfinding; };

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