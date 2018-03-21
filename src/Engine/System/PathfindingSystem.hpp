#pragma once

#include "System.hpp"
#include "Util/Memory.hpp"

//#include "../Component/PathfindingComponents/PathfindingComponent.hpp"

class Scene;
class PathfindingComponent;


// static class
class PathfindingSystem {

public:

	struct vecHash
	{
	  size_t operator()(const glm::vec3 &v) const {
	    size_t h1 = std::hash<int>()(int(round(v.x)));
	    size_t h2 = std::hash<int>()(int(round(v.y)));
	    size_t h3 = std::hash<int>()(int(round(v.z)));
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

    friend Scene;
    
    public:

    static void init();

    static void update(float dt);

    static vecvectorMap graph;

    private:

    static const Vector<PathfindingComponent *> & s_pathfindingComponents;

    static void readInGraph(String, vecvectorMap &);

};