#pragma once

#include <iostream>
#include <queue>

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include "Component/Component.hpp"

class PathfindingSystem;


struct vecHash
{
  size_t operator()(const glm::vec3 &v) const {
  	size_t h1 = std::hash<int>()(round(v.x));
	size_t h2 = std::hash<int>()(round(v.y));
	size_t h3 = std::hash<int>()(round(v.z));
	return (h1 ^ (h2 << 1)) ^ h3;
  }
};


struct gridCompare {

    bool operator()(const glm::vec3& lhs, const glm::vec3& rhs) const
    {
        if (round(lhs.x) == round(rhs.x) && round(lhs.z) == round(rhs.z)) {
            if (glm::distance(lhs.y, rhs.y) < .5) {
                return true;
            }
        }
        return false;
    }
};


typedef std::unordered_map<glm::vec3, glm::vec3, vecHash, gridCompare> vecvecMap;
typedef std::unordered_map<glm::vec3, double, vecHash, gridCompare> vecdoubleMap;
typedef std::unordered_map<glm::vec3, Vector<glm::vec3>, vecHash, gridCompare> vecvectorMap;


struct Node {
	glm::vec3 position;
	Vector<glm::vec3> neighbors;

	Node(glm::vec3 pos, Vector<glm::vec3> neighbors) :
		position(pos),
		neighbors(neighbors)
	{}
};

struct pathPair {
    glm::vec3 position;
    double priority;

    pathPair(glm::vec3 pos, double pri) :
        position(pos),
        priority(pri)
    {}
};


class PathfindingComponent : public Component {

    friend Scene;
    
    protected: // only scene or friends can create component

    PathfindingComponent(GameObject & gameObject, GameObject & player, float ms);
    //PathfindingComponent(GameObject & gameObject, GameObject & player, float ms, bool wander);

    public:

    PathfindingComponent(PathfindingComponent && other) = default;

    // 45 degrees
    static constexpr float k_defCriticalAngle = glm::pi<float>() * 0.25f;

    protected:

    virtual void init() override;

    void readInGraph(String);
    Vector<glm::vec3> reconstructPath(glm::vec3 start, glm::vec3 playerPos, vecvecMap &cameFrom);


    // cosine of most severe angle that can still be considered "ground"
    float m_cosCriticalAngle;

    public:

    virtual void update(float) override;

    static bool aStarSearch(vecvectorMap &graph, glm::vec3 start, glm::vec3 playerPos, vecvecMap &cameFrom);
    static glm::vec3 closestPos(vecvectorMap &graph, glm::vec3 pos);

    // TODO : just add enable/disable options for all components?
    void setMoveSpeed(float f) { this->m_moveSpeed = f; }

    private:

    SpatialComponent * m_spatial;
    const GameObject & m_player;
    float m_moveSpeed;

    //glm::vec3 m_groundNorm;
    //glm::vec3 m_potentialGroundNorm;

    //glm::vec3 curPos;
    //glm::vec3 searchFromPos;
    //bool nonGroundCollision;
    //bool writeOut;

    //int slowTime;
    //int dirIndex;
    //int yIndex; 
    //int *checkedDirections;

    //glm::vec3 prevMove;
    //std::queue<glm::vec3> pos_queue;
    //std::unordered_set<glm::vec3, vecHash, gridCompare> visitedSet;
    //Vector<glm::vec3> visitedSet;
    bool updatePath;
    int pathCount;
    bool noPath = false;

    vecvectorMap graph;
    //std::unordered_map<glm::vec3, Node, vecHash, gridCompare> vecToNode;
    vecvecMap cameFrom;
    Vector<glm::vec3> path;
    std::vector<glm::vec3>::iterator pathIT;

    //Vector<glm::vec3> validNeighbors;

    //bool m_wander;
    //glm::vec3 m_wanderCurrent;
    //float m_wanderCurrentWeight;
    //float m_wanderWeight;
};