#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"
#include <iostream>
#include <queue>

#include "Loader/Loader.hpp"

class PathfindingSystem;


namespace glm {
namespace detail {
	GLM_INLINE void hash_combine(size_t &seed, size_t hash)
	{
		//Magic number is the golden ratio used to randomize bits
		hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hash;
	}
}}

namespace std {
    template <typename T, glm::precision P>
	struct hash<glm::tvec3<T,P>>
	{
		GLM_FUNC_DECL size_t operator()(const glm::tvec3<T,P> &v) const;
	};

	template <typename T, glm::precision P>
	GLM_FUNC_QUALIFIER size_t
	std::hash<glm::tvec3<T,P>>::operator()(const glm::tvec3<T,P> &v) const
	{
		size_t seed = 0;
		hash<T> hasher;
		glm::detail::hash_combine(seed, hasher(v.x));
		glm::detail::hash_combine(seed, hasher(v.y));
		glm::detail::hash_combine(seed, hasher(v.z));
		return seed;
	}
}

struct customCompare {

	bool operator()(const glm::vec3& lhs, const glm::vec3& rhs)
	{
		float radius = 5.f;

		if (lhs.x < rhs.x + radius && lhs.x > rhs.x - radius) {
			if (lhs.y < rhs.y + radius && lhs.y > rhs.y - radius) {
				if (lhs.z < rhs.z + radius && lhs.z > rhs.z - radius) {
					return true;
				}
			}
		}

		return false;
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

    public:

    virtual SystemID systemID() const override { return SystemID::pathfinding; };

    virtual void update(float) override;

    private:

    SpatialComponent * m_spatial;
    GameObject * m_player;
    float m_moveSpeed;


    glm::vec3 curPos;
    bool prevCollision;

    int slowTime;
    int dirIndex;

    glm::vec3 prevMove;
    Vector<glm::vec3> curPosNeighbors;
    std::queue<glm::vec3> pos_queue;
    std::unordered_set<glm::vec3, std::hash<glm::vec3>, customCompare> visitedSet;
    UnorderedMap<glm::vec3, Vector<glm::vec3>> graph;

};