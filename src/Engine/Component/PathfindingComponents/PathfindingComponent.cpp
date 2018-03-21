#include "PathfindingComponent.hpp"

#include "Scene/Scene.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/CollisionComponents/BounderComponent.hpp"
#include "System/CollisionSystem.hpp"
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
    const glm::vec3 &pos = m_bounder->groundPosition();

    updatePath = true;
}

void PathfindingComponent::update(float dt) {
    float HALF_STAIRS = 3.f;
    const glm::vec3 & playerPos = m_player.getSpatial()->position();
    const glm::vec3 & pos = m_bounder->groundPosition();

    glm::vec3 dir = playerPos - pos;

    glm::vec3 playerGroundPos = playerPos;
    auto pair(CollisionSystem::pickHeavy(Ray(playerPos, glm::vec3(0, -1, 0.01)), UINT_MAX));
    if (pair.second.is) {
        //playerGroundPos = glm::vec3(playerPos.x, playerPos.y - pair.second.dist, playerPos);
        playerGroundPos.y -= pair.second.dist;
    }


    // if enemy is very close to the player just follow them
    //if (glm::length2(dir) < 5.0 || noPath) {
    float dist = glm::distance2(pos, playerPos);
    if (dist < 300.0 && abs(pos.y - playerGroundPos.y) < HALF_STAIRS) {

        gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);

        if (pathCount++ > 50) {
            updatePath = true;
            noPath = false;
            pathCount = 0;
        }

    }
    // probably don't need to update the path everytime, set flag when neccessary
    else if (updatePath) {
        if (aStarSearch(PathfindingSystem::graph, pos, playerGroundPos, cameFrom)) {
            path = reconstructPath(pos, playerGroundPos, cameFrom);
            pathIT = path.begin();

            noPath = false;

            dir = *pathIT - pos;
        }
        else {
            noPath = true;
        }

        gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);
        updatePath = false;
    }
    else {
        if (glm::length2(*pathIT - pos) < 1.0f) {
            if (std::next(pathIT) == path.end()) {
                updatePath = true;
                pathCount = 0;
            }
            else {
                pathIT++;
            }
        }

        dir = *pathIT - pos;

        gameObject().getSpatial()->move(Util::safeNorm(dir) * m_moveSpeed * dt);

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

    start = PathfindingComponent::closestPos(graph, start);
    end = PathfindingComponent::closestPos(graph, end);

    frontier.emplace(start, 0.0);

    cameFrom[start] = start;
    cost[start] = 0.0;

    while (!frontier.empty()) {
        glm::vec3 current = frontier.top().position;
        frontier.pop();


        if (current == end) {

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

    return false;

}

Vector<glm::vec3> PathfindingComponent::reconstructPath(glm::vec3 start, glm::vec3 end, PathfindingSystem::vecvecMap &cameFrom) {
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
        return path;
    }

    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

glm::vec3 PathfindingComponent::closestPos(PathfindingSystem::vecvectorMap &graph, glm::vec3 pos) {
    float HALF_STAIRS = 3.f;
    float minDist = FLT_MAX;
    glm::vec3 minPos = glm::vec3();

    for (auto iter = graph.begin(); iter != graph.end(); ++iter) {
        float dist = glm::distance2(pos, iter->first);
        if (dist < minDist && abs(pos.y - iter->first.y) < HALF_STAIRS) {
            minPos = iter->first;
            minDist = dist;

        }
    }

    return minPos;
}


