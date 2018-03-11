#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"
#include <iostream>
#include <queue>

#include "Loader/Loader.hpp"

class PathfindingSystem;

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


	struct customCompare {

		bool operator()(const glm::vec3& lhs, const glm::vec3& rhs)
		{
			float radius = 1.f;

			if (glm::distance2(lhs, rhs) > radius * radius) {
				return false;
			}
			return true;
		}
	};
}

struct Node {
	glm::vec3 position;
	Vector<glm::vec3> neighbors;

	Node(glm::vec3 pos, Vector<glm::vec3> neighbors) :
		position(pos),
		neighbors(neighbors)
	{}
};


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

    SpatialComponent * m_spatial;
    GameObject * m_player;
    float m_moveSpeed;

    glm::vec3 m_groundNorm;
    glm::vec3 m_potentialGroundNorm;

    glm::vec3 curPos;
    glm::vec3 searchFromPos;
    bool nonGroundCollision;
    bool writeOut;

    int slowTime;
    int dirIndex;
    int yIndex; 
    int *checkedDirections;

    glm::vec3 prevMove;
    std::queue<glm::vec3> pos_queue;
    std::unordered_set<glm::vec3, detail::vecHash, detail::customCompare> visitedSet;
    //Vector<glm::vec3> visitedSet;
    Vector<Node> graph;
    Vector<glm::vec3> validNeighbors;

    bool m_wander;
    glm::vec3 m_wanderCurrent;
    float m_wanderCurrentWeight;
    float m_wanderWeight;
};