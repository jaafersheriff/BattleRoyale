#include "PathfindingComponent.hpp"

#include <iostream>

#include "Scene/Scene.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Util/Util.hpp"
#include "Loader/Loader.hpp"


PathfindingComponent::PathfindingComponent(GameObject & gameObject, GameObject & player, float ms) :
	Component(gameObject),
    m_spatial(nullptr),
    m_player(&player),
    m_moveSpeed(ms)
{}

void PathfindingComponent::init() {

    // Init graph
    graph = vecvectorMap();

    // init spatial
    if (!(m_spatial = gameObject().getSpatial())) assert(false);

    //vecToNode = std::unordered_map<glm::vec3, Node, vecHash, gridCompare>();

    // Read in the graph of the map from a text file
    readInGraph("../resources/newMap.txt");

    // init cameFrom map
    cameFrom = vecvecMap();

    const glm::vec3 &playerPos = m_player->getSpatial()->position();
    const glm::vec3 &pos = m_spatial->position();

    std::cout << "Start: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;

    std::cout << "Distance: " << glm::distance(glm::vec3(0, 0, 0), glm::vec3(1, 0, 1)) << std::endl;

    //glm::vec3 test = glm::vec3(4, -1.68, -16);

    if (aStarSearch(graph, pos, playerPos, cameFrom)) {
        path = reconstructPath(pos, playerPos, cameFrom);
        pathIT = path.begin();
    }
    else {
        std::cout << "Init: aStart didn't find a path" << std::endl;
    }

    // std::cout << "Path" << std::endl;
    // for (auto iter = path.begin(); iter != path.end(); ++iter) {
    //    std::cout << iter->x << ", " << iter->y << ", " << iter->z << std::endl;
    // }

    updatePath = false;

    std::cout << "End Init" << std::endl;
}

void PathfindingComponent::update(float dt) {

    //std::cout << "Update start" << std::endl;
    const glm::vec3 & playerPos = m_player->getSpatial()->position();
    const glm::vec3 & pos = m_spatial->position();
    glm::vec3 dir = playerPos - pos;

    // Could do a ray pick to see if the player is ahead unobstructed, but then what if the player is on the second floor railing

    // if enemy is very close to the player just follow them
    //std::cout << "Dir length: " << glm::length2(dir) << std::endl;
    if (glm::length2(dir) < 5.0 || noPath) {
        //std::cout << "close to player: " << glm::length2(dir) << std::endl;
        gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);

        if (pathCount++ > 50) {
            updatePath = true;
            noPath = false;
            pathCount = 0;
        }

    }
    // probably don't need to update the path everytime, set flag when neccessary
    else if (updatePath) {
        std::cout << "update path" << std::endl;

        if (aStarSearch(graph, pos, playerPos, cameFrom)) {
            path = reconstructPath(pos, playerPos, cameFrom);
            pathIT = path.begin();

            noPath = false;

            dir = *pathIT - pos;
        }
        else {
            std::cout << "***************aStart didn't find a path" << std::endl;
            noPath = true;
        }
        gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);
        updatePath = false;
    }
    else {
        //std::cout << "walk on path" << std::endl;
        if (glm::length2(*pathIT - pos) < 1.0) {
            pathIT++;
        }

        dir = *pathIT - pos;
        gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);

        if (pathCount++ > 50) {
            updatePath = true;
            pathCount = 0;
        }

        //std::cout << "walked on path" << std::endl;

    }
    
    //std::cout << "Update End" << std::endl;
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

            if (neighbors.size()) {
                std::cout << "More Than 8 neighbors" << std::endl;
            }

            graph.emplace(nodePos, neighbors);
            //vecToNode.emplace(nodePos, Node(nodePos, neighbors));
        }

        myfile.close();

        std::cout << "Size: " << graph.size() << std::endl;
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

inline double heuristic(glm::vec3 a, glm::vec3 b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
}

bool operator > (pathPair a , pathPair b) {
    return a.priority > b.priority;
}

bool operator == (glm::vec3 a, glm::vec3 b) {
    //std::cout << "Operator: " << a.x << ", " << a.y << ", " << a.z << "|" << b.x << ", " << b.y << ", " << b.z << "dist: " << glm::distance2(a, b) << std::endl;
    //if (a.x == b.x && a.z == b.z) {
    //    std::cout << "Operator: " << a.x << ", " << a.y << ", " << a.z << "|" << b.x << ", " << b.y << ", " << b.z << "dist: " << glm::distance2(a, b) << std::endl;
    //}
    return glm::distance2(a, b) < 1.f;
}

bool operator != (glm::vec3 a, glm::vec3 b) {
    return !(a == b);
}

bool PathfindingComponent::aStarSearch(vecvectorMap &graph, glm::vec3 start, glm::vec3 end, vecvecMap &cameFrom) {//, vecdoubleMap &cost) {
    //typedef std::pair<double, glm::vec3> pqElement;
    vecdoubleMap cost = vecdoubleMap();
    std::priority_queue<pathPair, Vector<pathPair>, std::greater<pathPair>> frontier;

    start = PathfindingComponent::closestPos(graph, start);
    end = PathfindingComponent::closestPos(graph, end);

    frontier.emplace(start, 0.0);

    cameFrom[start] = start;
    cost[start] = 0.0;

    std::cout << "In A*" << std::endl;
    while (!frontier.empty()) {
        //std::cout << "LOOP" << std::endl;
        glm::vec3 current = frontier.top().position;
        frontier.pop();

        //std::cout << "current: " << current.x << ", " << current.y << ", " << current.z << std::endl;

        if (current == end) {
            std::cout << "leaving A*" << std::endl;
            return true;
        }

        for (glm::vec3 next : graph[current]) {
            // We aren't using a weighted graph so every step has a cost of 1
            double newCost = cost[current] + 1;
            if (cost.find(next) == cost.end() || newCost < cost[next]) {
                cost[next] = newCost;
                // get neighbor node
                frontier.emplace(next, newCost + heuristic(next, end));
                cameFrom[next] = current;
            }
        }
    }

    std::cout << "leaving A*" << std::endl;
    return false;
/*
std::cout << "CamFrom" << std::endl;
    for (auto iter = cameFrom.begin(); iter != cameFrom.end(); ++iter) {
         std::cout << "Postition: " << iter->first.x << ", " << iter->first.y << ", " << iter->first.z << " == " << iter->second.x << ", " << iter->second.y << ", " << iter->second.z << std::endl;
    }
*/

 

}

Vector<glm::vec3> PathfindingComponent::reconstructPath(glm::vec3 start, glm::vec3 end, vecvecMap &cameFrom) {
    std::cout << "In RePath" << std::endl;
    Vector<glm::vec3> path;
    int count = 0;

    start = PathfindingComponent::closestPos(graph, start);
    end = PathfindingComponent::closestPos(graph, end);

    glm::vec3 current = end;

    std::cout << "Start: " << start.x << ", " << start.y << ", " << start.z << std::endl;
    std::cout << "Current: " << end.x << ", " << end.y << ", " << end.z << std::endl;
    //std::cout << "Size: " << cameFrom.size() << std::endl;

    while (current != start && count < cameFrom.size()) {
        count++;
        path.push_back(current);
        current = cameFrom[current];
        //if (current != glm::vec3(0.0))
        //    std::cout << "New Current: " << current.x << ", " << current.y << ", " << current.z << std::endl;
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    std::cout << "Out RePath" << std::endl;
    return path;
}

glm::vec3 PathfindingComponent::closestPos(vecvectorMap &graph, glm::vec3 pos) {
    std::cout << "in closestPos" << std::endl;
    float minDist = FLT_MAX;
    glm::vec3 minPos = glm::vec3();
    glm::vec3 rounded = glm::vec3(round(pos.x), pos.y, round(pos.z));

    if (graph.find(glm::vec3(rounded)) != graph.end()) {
        std::cout << "Returned rounded" << std::endl;
        return rounded;
    }

    for (auto iter = graph.begin(); iter != graph.end(); ++iter) {
        float dist = glm::distance2(pos, iter->first);
        if (dist < minDist) {
            // If there is a pos this close don't keep checking
            if (dist < 1.f) {
                return iter->first;
            }
            minPos = iter->first;
            minDist = dist;

        }
    }

    if (minPos == glm::vec3()) {
        std::cout << "Something is off: PathfindingComponent::closestPos" << std::endl;
    }

    std::cout << "out closestPos" << std::endl;

    return minPos;
}


