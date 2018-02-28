#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"
#include <iostream>
#include <queue>

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


class PathfindingComponent : public Component {

    friend Scene;
    
    protected: // only scene or friends can create component

    PathfindingComponent(GameObject & player, float ms);

    public:

    PathfindingComponent(PathfindingComponent && other) = default;

    protected:

    virtual void init(GameObject & go) override;

    public:

    virtual SystemID systemID() const override { return SystemID::pathfinding; };

    virtual void update(float) override;

    private:

    SpatialComponent * m_spatial;
    GameObject * m_player;
    float m_moveSpeed;


    struct node
    {
    	int updateCount;
    	glm::vec3 wPos;
    };

    const std::queue<node> map;
    const std::unordered_map<glm::vec3, glm::vec3[]> graph;

};