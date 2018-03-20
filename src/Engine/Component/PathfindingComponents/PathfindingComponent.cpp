#include "PathfindingComponent.hpp"

#include "Scene/Scene.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "Util/Util.hpp"
#include "Loader/Loader.hpp"

//#include "System/PathfindingSystem.hpp"

PathfindingComponent::PathfindingComponent(GameObject & gameObject, GameObject & player, float ms) :
	Component(gameObject),
    m_spatial(nullptr),
    m_player(player),
    m_moveSpeed(ms)
{}

void PathfindingComponent::init() {

    // init spatial
    if (!(m_spatial = gameObject().getSpatial())) assert(false);

    if (!(m_bounder = gameObject().getComponentByType<BounderComponent>())) assert(false);

    // init cameFrom map
    cameFrom = PathfindingSystem::vecvecMap();

    const glm::vec3 &playerPos = m_player.getSpatial()->position();
    //const glm::vec3 &pos = m_spatial->position();
    const glm::vec3 &pos = m_bounder->groundPosition();

    std::cout << "Start: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;

    std::cout << "Distance: " << glm::distance(glm::vec3(0, 0, 0), glm::vec3(1, 0, 1)) << std::endl;

    if (aStarSearch(PathfindingSystem::graph, pos, playerPos, cameFrom)) {
        path = reconstructPath(pos, playerPos, cameFrom);
        pathIT = path.begin();
    }
    else {
        std::cout << "Init: aStart didn't find a path" << std::endl;
    }

    updatePath = false;

    std::cout << "End Init" << std::endl;
}

void PathfindingComponent::update(float dt) {
    const glm::vec3 & playerPos = m_player.getSpatial()->position();
    const glm::vec3 & pos = m_bounder->groundPosition();

    glm::vec3 dir = playerPos - pos;


    // if enemy is very close to the player just follow them
    if (glm::length2(dir) < 5.0 || noPath) {

        gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);

        if (pathCount++ > 50) {
            updatePath = true;
            noPath = false;
            pathCount = 0;
        }

    }
    // probably don't need to update the path everytime, set flag when neccessary
    else if (updatePath) {
        if (aStarSearch(PathfindingSystem::graph, pos, playerPos, cameFrom)) {
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
        // move on to the next node if you close to the current one
        if (glm::length2(*pathIT - pos) < 1.0) {
            pathIT++;
        }
        dir = *pathIT - pos;

        gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);

        // update the path after 50 update loops or when the path is complete
        if (pathCount++ > 50 || std::next(pathIT) == path.end()) {
            updatePath = true;
            pathCount = 0;
        }

    }
    
}

inline double heuristic(glm::vec3 a, glm::vec3 b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
}

bool operator > (pathPair a , pathPair b) {
    return a.priority > b.priority;
}

bool operator == (glm::vec3 a, glm::vec3 b) {

    return glm::distance2(a, b) < 1.f;
}

bool operator != (glm::vec3 a, glm::vec3 b) {
    return !(a == b);
}

bool PathfindingComponent::aStarSearch(PathfindingSystem::vecvectorMap &graph, glm::vec3 start, glm::vec3 end, PathfindingSystem::vecvecMap &cameFrom) {
    PathfindingSystem::vecdoubleMap cost = PathfindingSystem::vecdoubleMap();
    std::priority_queue<pathPair, Vector<pathPair>, std::greater<pathPair>> frontier;

    std::cout << "Pos as start in aStar: " << start.x << ", " << start.y << ", " << start.z << std::endl;
    start = PathfindingComponent::closestPos(graph, start);
    end = PathfindingComponent::closestPos(graph, end);

    frontier.emplace(start, 0.0);

    cameFrom[start] = start;
    cost[start] = 0.0;

    std::cout << "In A*" << std::endl;
    while (!frontier.empty()) {
        glm::vec3 current = frontier.top().position;
        frontier.pop();


        if (current == end) {

            std::cout << "true leaving A*" << std::endl;
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

    std::cout << "false leaving A*" << std::endl;
    return false;

}

Vector<glm::vec3> PathfindingComponent::reconstructPath(glm::vec3 start, glm::vec3 end, PathfindingSystem::vecvecMap &cameFrom) {
    std::cout << "In RePath" << std::endl;
    Vector<glm::vec3> path;
    int count = 0;

    start = PathfindingComponent::closestPos(PathfindingSystem::graph, start);
    end = PathfindingComponent::closestPos(PathfindingSystem::graph, end);

    glm::vec3 current = end;

    while (current != start && count < cameFrom.size()) {
        count++;
        path.push_back(current);
        current = cameFrom[current];
    }

    // If path not reconstructed and while loop was exited because of count just return end
    if (count > cameFrom.size()) {
        path = Vector<glm::vec3>();
        path.push_back(end);
        std::cout << "Path not reconstructed: just end returned" << std::endl;
        return path;
    }

    path.push_back(start);
    std::reverse(path.begin(), path.end());
    std::cout << "Out RePath" << std::endl;
    return path;
}

glm::vec3 PathfindingComponent::closestPos(PathfindingSystem::vecvectorMap &graph, glm::vec3 pos) {
    std::cout << "Start Pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
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
        std::cout << "After Pos: " << pos.x << ", " << pos.y << ", " << pos.z << ", graph size: " << graph.size() << std::endl;
        std::cout << "Something is off: PathfindingComponent::closestPos" << std::endl;
    }

    std::cout << "out closestPos" << std::endl;

    return minPos;
}


