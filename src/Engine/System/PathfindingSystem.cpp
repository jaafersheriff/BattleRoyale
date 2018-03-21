#include "PathfindingSystem.hpp"

#include "Scene/Scene.hpp"
#include "Component/PathfindingComponents/PathfindingComponent.hpp"
#include <fstream>
#include <sstream>

// Init graph
PathfindingSystem::vecvectorMap PathfindingSystem::graph = PathfindingSystem::vecvectorMap();

const Vector<PathfindingComponent *> & PathfindingSystem::s_pathfindingComponents(Scene::getComponents<PathfindingComponent>());

void PathfindingSystem::init() {

    // Read in the graph of the map from a text file
    readInGraph("../resources/smallMap.txt", graph);
}

void PathfindingSystem::update(float dt) {
    for (auto & comp : s_pathfindingComponents) {
        comp->update(dt);
    }
}

// Read in the graph from a specified file and fill out the vecToNode map
void PathfindingSystem::readInGraph(String fileName, vecvectorMap &graph) {
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
        }

    }

}