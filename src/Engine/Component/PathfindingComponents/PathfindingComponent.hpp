#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"
#include <iostream>
#include <queue>

#include "Loader/Loader.hpp"

class PathfindingSystem;


struct customCompare {

	bool operator()(const glm::vec3& lhs, const glm::vec3& rhs)
	{
		float radius = 2.5f;

		std::cout << "Left: " <<  lhs.x << ", " << lhs.y << ", " << lhs.z << std::endl;
		std::cout << "Right: " <<  rhs.x << ", " << rhs.y << ", " << rhs.z << std::endl;

		if (lhs.x < rhs.x + radius && lhs.x > rhs.x - radius) {
			if (lhs.y < rhs.y + radius && lhs.y > rhs.y - radius) {
				if (lhs.z < rhs.z + radius && lhs.z > rhs.z - radius) {
					
					std::cout << "customCompare: true" << std::endl;
					return false;
				}
			}
		}

		std::cout << "customCompare: false" << std::endl;
		return true;
	}
};



class PathfindingComponent : public Component {

    friend Scene;
    
    protected: // only scene or friends can create component

    PathfindingComponent(GameObject & gameObject, GameObject & player, float ms);

    public:

    PathfindingComponent(PathfindingComponent && other) = default;

    protected:

    virtual void init() override;

    void print_queue(std::queue<glm::vec3> q);
    void drawCup(glm::vec3 position);
    bool findInVisited(glm::vec3 vec);

    public:

    virtual SystemID systemID() const override { return SystemID::pathfinding; };

    virtual void update(float) override;

    private:

    SpatialComponent * m_spatial;
    GameObject * m_player;
    float m_moveSpeed;


    glm::vec3 curPos;
    glm::vec3 searchFromPos;
    bool prevCollision;

    int slowTime;
    int dirIndex;

    glm::vec3 prevMove;
    Vector<glm::vec3> curPosNeighbors;
    std::queue<glm::vec3> pos_queue;
    //std::set<glm::vec3, customCompare> visitedSet;
    Vector<glm::vec3> visitedSet;
    //UnorderedMap<glm::vec3, Vector<glm::vec3>> graph;

};