#pragma once
#ifndef _MAPEXPLORE_COMPONENT
#define _MAPEXPLORE_COMPONENT

#include "Component/Component.hpp"
#include "Component/PathfindingComponents/PathfindingComponent.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/string_cast.hpp"

#include <iostream>
#include <queue>

#include "Loader/Loader.hpp"

#include "Util/Util.hpp"




class Scene;
class SpatialComponent;
class MapExploreSystem;
class CollisionSystem;


class MapExploreComponent : public Component {

    friend Scene;
    
    protected: // only scene or friends can create component

    MapExploreComponent(GameObject & gameObject, float ms, String filename);

    public:

    MapExploreComponent(MapExploreComponent && other) = default;

    // 45 degrees
    static constexpr float k_defCriticalAngle = glm::pi<float>() * 0.25f;

    protected:

    virtual void init() override;

    //void aStarSearch(detail::vecvectorMap &graph, glm::vec3 start, glm::vec3 end, detail::vecvecMap &cameFrom);//, detail::vecdoubleMap &cost);
    //Vector<glm::vec3> reconstructPath(glm::vec3 start, glm::vec3 end, detail::vecvecMap &cameFrom);
    //void readInGraph(String fileName); 
    //void print_queue(std::queue<glm::vec3> q);
    //void drawCup(glm::vec3 position);
    void writeToFile(vecvectorMap &graph); 
    bool findInVisited(glm::vec3 vec, float stepSize);
    std::string vectorToString(Vector<glm::vec3> vec);
    glm::vec3 closestPos(glm::vec3 vec);
    void drawCup(glm::vec3 position);

    bool removeOutliers(std::unordered_set<glm::vec3, vecHash, gridCompare> &graphSet);
    bool validNeighbor(glm::vec3 curPos, glm::vec3 candidate);
    Vector<glm::vec3> gridFind(std::unordered_set<glm::vec3, vecHash, gridCompare> &graphSet, int xPos, int zPos);

    // cosine of most severe angle that can still be considered "ground"
    float m_cosCriticalAngle;

    public:

    virtual SystemID systemID() const override { return SystemID::mapexplore; };

    virtual void update(float) override;

    // TODO : just add enable/disable options for all components?
    void setMoveSpeed(float f) { this->m_moveSpeed = f; }

    //static String FILENAME; // filename of the graph to be written in the build dir

    private:

    //typedef std::unordered_map<glm::vec3, glm::vec3, detail::vecHash, detail::gridCompare> vecvecMap;
    //typedef std::unordered_map<glm::vec3, double, detail::vecHash, detail::gridCompare> vecdoubleMap;

    SpatialComponent * m_spatial;
    GameObject * m_player;
    float m_moveSpeed;
    String filename;

    glm::vec3 m_groundNorm;
    glm::vec3 m_potentialGroundNorm;

    glm::vec3 curPos;
    glm::vec3 searchFromPos;
    bool nonGroundCollision;
    bool writeOut;
    bool oneUpdate = false;
    

    int secondFloorWidth = 97;
    int secondFloorDepth = 208;

    int firstFloorWidth = 73;
    int firstFloorDepth = 190;

    int xIndex = 0;
    int zIndex = 0;

    int secondFloorStart_x = -48;
    int secondFloorStart_z = -186;
    int firstFloorStart_x = -35;
    int firstFloorStart_z = -179;

    int secondFloorHeight = 10;
    int firstFloorHeight = 2;

    bool secondFloorPass = true;
    bool firstFloorPass = false;
    bool collisionCheck = false;
    bool findNeighbors = false;

    int collisionCount = 0;
    std::unordered_set<glm::vec3, vecHash, gridCompare>::iterator visitIterator;
    glm::vec3 collisionTestPoint;
    std::unordered_set<glm::vec3, vecHash, gridCompare> visitedSet;
    std::unordered_set<glm::vec3, vecHash, gridCompare> graphSet;

    int nodeCount = 0;


    int slowTime;
    int dirIndex;
    int yIndex; 
    int *checkedDirections;

    glm::vec3 prevMove;
    std::queue<glm::vec3> pos_queue;
    
    vecvectorMap graph;
    Vector<glm::vec3> validNeighbors;



};

#endif