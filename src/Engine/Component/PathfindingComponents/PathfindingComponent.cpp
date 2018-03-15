#include "PathfindingComponent.hpp"

#include <iostream>

#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Util/Util.hpp"
#include "Loader/Loader.hpp"


PathfindingComponent::PathfindingComponent(GameObject & gameObject, GameObject & player, float ms) :
	Component(gameObject),
    m_spatial(nullptr),
    m_player(&player),
    m_moveSpeed(ms)
{}

PathfindingComponent::PathfindingComponent(GameObject & gameObject, GameObject & player, float ms, bool wander) :
    Component(gameObject),
    m_spatial(nullptr),
    m_player(&player),
    m_moveSpeed(ms),
    m_wander(wander),
    m_wanderCurrent(0.0f, 0.0f, 0.0f),
    m_wanderCurrentWeight(0.9f),
    m_wanderWeight(0.9f)
{}

void PathfindingComponent::init() {

    // Init graph
    graph = detail::vecvectorMap();

    // init spatial
    if (!(m_spatial = gameObject().getSpatial())) assert(false);

    vecToNode = std::unordered_map<glm::vec3, Node, detail::vecHash, detail::customVecCompare>();

    // Read in the graph of the map from a text file
    readInGraph("map.txt");

    // init cameFrom map
    cameFrom = detail::vecvecMap();

    const glm::vec3 &playerPos = m_player->getSpatial()->position();
    const glm::vec3 &pos = m_spatial->position();

    aStarSearch(graph, pos, playerPos, cameFrom);
    path = reconstructPath(pos, playerPos, cameFrom);
    pathIT = path.begin();

    updatePath = false;

    std::cout << "End Init" << std::endl;
}

void PathfindingComponent::update(float dt) {
/*
    std::cout << "Update start" << std::endl;
    const glm::vec3 & playerPos = m_player->getSpatial()->position();
    const glm::vec3 & pos = m_spatial->position();
    glm::vec3 dir = playerPos - pos;

    // Could do a ray pick to see if the player is ahead unobstructed, but then what if the player is on the second floor railing

    // if enemy is very close to the player just follow them
    if (glm::length2(dir) < 1.0) {
        std::cout << "close to player: " << glm::length2(dir) << std::endl;
        gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);
    }
    // probably don't need to update the path everytime, set flag when neccessary
    else if (updatePath) {
        std::cout << "update path" << std::endl;
        aStarSearch(graph, pos, playerPos, cameFrom);
        path = reconstructPath(pos, playerPos, cameFrom);
        pathIT = path.begin();

        dir = *pathIT - pos;
        gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);

        updatePath = false;
    }
    else {
        std::cout << "walk on path" << std::endl;
        if (glm::length2(*pathIT - pos) < 1.0) {
            pathIT++;
        }

        dir = *pathIT - pos;
        gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);

        if (pathCount++ > 50) {
            updatePath = true;
            pathCount = 0;
        }

        std::cout << "walked on path" << std::endl;

    }
    */
}

// Read in the graph from a specified file and fill out the vecToNode map
void PathfindingComponent::readInGraph(String fileName) {
    std::ifstream myfile(fileName.c_str());
    std::string line;

    if (myfile.is_open()) {
        while (getline(myfile, line)) {
            std::istringstream iss(line);
            std::string token;
            float x, y, z;

            std::getline(iss, token, ',');
            x = std::stof(token);

            std::getline(iss, token, ',');
            y = std::stof(token);

            std::getline(iss, token, ',');
            z = std::stof(token);

            glm::vec3 nodePos = glm::vec3(x, y, z);

            Vector<glm::vec3> neighbors = Vector<glm::vec3>();

            while (std::getline(iss, token, ',')) {
                int pos = token.find("vec3");
                if (pos != std::string::npos) {

                    x = std::stof(token.substr(5, 8));

                    std::getline(iss, token, ',');
                    y = std::stof(token.substr(1, 8));

                    std::getline(iss, token, ',');
                    z = std::stof(token.substr(1, 8));

                    neighbors.push_back(glm::vec3(x, y, z));

                }
            }

            graph.emplace(nodePos, neighbors);
            vecToNode.emplace(nodePos, Node(nodePos, neighbors));
        }

        myfile.close();
/*
        int graphCount = 0;
        for (auto iter = graph.begin(); iter != graph.end(); ++iter) {
            graphCount++;
            std::cout << "New Node" << std::endl;
            std::cout << "Position: " << iter->first.x << ", " << iter->first.y << ", " << iter->first.z << std::endl;
            std::cout << "Neighbors" << std::endl;
            for (glm::vec3 neighbor : iter->second) {
                std::cout << "Pos: " << neighbor.x << ", " << neighbor.y << ", " << neighbor.z << std::endl;
            }
            std::cout << std::endl;
        }

        std::cout << "Size of map: " << graphCount << std::endl;
*/
    }

}

std::string PathfindingComponent::vectorToString(Vector<glm::vec3> vec) {
	std::string ret = "";

	for (glm::vec3 tmpVec : vec) {
		ret += glm::to_string(tmpVec) + ",";
	}

	return ret;
}

glm::vec3 PathfindingComponent::closestPos(glm::vec3 vec) {
	float minDist = 100000.f;
	glm::vec3 closeNode = glm::vec3();

	for (glm::vec3 node : visitedSet) {
		float dist = glm::distance2(node, vec);
		if (dist < minDist) {
			minDist = dist;
			closeNode = node;
		}
	}

	if (closeNode == glm::vec3()) {
		std::cout << "found nothing" << std::endl;
	}

	return closeNode;
}

/*
bool PathfindingComponent::findInVisited(glm::vec3 vec, float stepSize) {
	float epsilon = sqrt(2) * stepSize / 2.f;

	//for (glm::vec3 test : visitedSet) {
	for (std::vector<glm::vec3>::reverse_iterator test = visitedSet.rbegin(); test != visitedSet.rend(); ++test) {
		if (glm::distance2(glm::vec3(test->x, test->y, test->z), vec) < epsilon * epsilon) {
			return true;
		}
	}

	return false;
}
*/
/*
void PathfindingComponent::print_queue(std::queue<glm::vec3> q)
{
  while (!q.empty())
  {
    std::cout << glm::to_string(q.front()) << " ";
    q.pop();
  }
  std::cout << std::endl;
}

*/

inline double heuristic(glm::vec3 a, glm::vec3 b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
}

bool operator < (pathPair a , pathPair b) {
    return a.priority < b.priority;
}

bool operator == (glm::vec3 a, glm::vec3 b) {
    return glm::distance2(a, b) < .1f;
}

bool operator != (glm::vec3 a, glm::vec3 b) {
    return !(a == b);
}

void PathfindingComponent::aStarSearch(detail::vecvectorMap &graph, glm::vec3 start, glm::vec3 end, detail::vecvecMap &cameFrom) {//, detail::vecdoubleMap &cost) {
    //typedef std::pair<double, glm::vec3> pqElement;
    detail::vecdoubleMap cost = detail::vecdoubleMap();
    std::priority_queue<pathPair> frontier;
    frontier.emplace(start, 0.0);

    cameFrom[start] = start;
    cost[start] = 0.0;

    std::cout << "In A*" << std::endl;
    while (!frontier.empty()) {
        //std::cout << "LOOP" << std::endl;
        glm::vec3 current = frontier.top().position;
        frontier.pop();

        if (current == end) {
            break;
        }

        for (glm::vec3 next : graph[current]) {
            // We aren't using a weighted graph so every step has a cost of 1
            double newCost = cost[current] + 1;
            if (cost.find(next) == cost.end() || newCost < cost[next]) {
                cost[next] = newCost;
                // get neighbor node
                frontier.emplace(vecToNode[next].position, newCost + heuristic(next, end));
                cameFrom[next] = current;
            }
        }
    }

    for (auto iter = cameFrom.begin(); iter != cameFrom.end(); ++iter) {
         std::cout << "Postition: " << iter->first.x << ", " << iter->first.y << ", " << iter->first.z << std::endl;
    }

    std::cout << "leaving A*" << std::endl;

}

Vector<glm::vec3> PathfindingComponent::reconstructPath(glm::vec3 start, glm::vec3 end, detail::vecvecMap &cameFrom) {
    std::cout << "Reconstructing Path" << std::endl;
    Vector<glm::vec3> path;

    glm::vec3 current = end;

    std::cout << "Current: " << start.x << ", " << start.y << ", " << start.z << std::endl;
    std::cout << "End: " << end.x << ", " << end.y << ", " << end.z << std::endl;
    std::cout << "Size: " << cameFrom.size() << std::endl;

    while (current != start) {
        path.push_back(current);
        current = cameFrom[current];
        if (current != glm::vec3(0.0))
            std::cout << "New Current: " << current.x << ", " << current.y << ", " << current.z << std::endl;
    }
    //path.push_back(start);
    std::reverse(path.begin(), path.end());
    std::cout << "Path constructed" << std::endl;
    return path;
}


