#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"
#include <iostream>
#include <queue>

#include "Loader/Loader.hpp"

class PathfindingSystem;

namespace detail {

  inline void hash_combine(size_t &seed, size_t hash)
  {
    //Magic number is the golden ratio used to randomize bits
    hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= hash;
  }


	struct vecHash
	{
	  size_t operator()(const glm::vec3 &v) const {
	  	static std::string s_scratch(sizeof(glm::vec3), 0);
	  	std::memcpy(const_cast<char *>(s_scratch.c_str()), &v, sizeof(glm::vec3));
	  	return std::hash<std::string>()(s_scratch);
	    /*size_t seed = 0;
	    std::hash<float> hasher;
	    hash_combine(seed, hasher(v.x));
	    hash_combine(seed, hasher(v.y));
	    hash_combine(seed, hasher(v.z));
	    return seed;*/
	  }
	};


	struct customCompare {

		bool operator()(const glm::vec3& lhs, const glm::vec3& rhs)
		{
			float radius = .5f;

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

    public:

    PathfindingComponent(PathfindingComponent && other) = default;

    protected:

    virtual void init() override;

    void print_queue(std::queue<glm::vec3> q);
    void drawCup(glm::vec3 position);
    bool findInVisited(glm::vec3 vec);
    std::string vectorToString(Vector<glm::vec3> vec);

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
    std::queue<glm::vec3> pos_queue;
    //std::unordered_set<glm::vec3, detail::vecHash, detail::customCompare> visitedSet;
    Vector<glm::vec3> visitedSet;
    Vector<Node> graph;
    Vector<glm::vec3> validNeighbors;

};