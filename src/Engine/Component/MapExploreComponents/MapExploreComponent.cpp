#include "MapExploreComponent.hpp"

#include "Scene/Scene.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "System/MapExploreSystem.hpp"
#include "System/CollisionSystem.hpp"

MapExploreComponent::MapExploreComponent(GameObject & gameObject, float ms, String filename) :
	Component(gameObject),
	m_spatial(nullptr),
	m_moveSpeed(ms),
	filename(filename)
{}

void MapExploreComponent::init() {

    if (!(m_spatial = gameObject().getSpatial())) assert(false);

    slowTime = 0;

    visitedSet = std::unordered_set<glm::vec3, vecHash, gridCompare>();
    graphSet = std::unordered_set<glm::vec3, vecHash, gridCompare>();
    //visitedSet = Vector<glm::vec3>();
    pos_queue = std::queue<glm::vec3>();
    //curPos = m_spatial->position();
    //std::cout << "curPos: " << curPos.x << ", " << curPos.y << ", " << curPos.z << std::endl;

    //auto pair(CollisionSystem::pick(Ray(curPos, glm::vec3(0,-1,0)), &gameObject()));

    glm::vec3 tmpPos = gameObject().getSpatial()->position();
    //tmpPos = glm::vec3(tmpPos.x, tmpPos.y - pair.second.dist, tmpPos.z);

    // Node for the starting pos
    //visitedSet.insert(tmpPos);

    // Set curPos to the objects initial pos
    searchFromPos = tmpPos;
    //std::cout << "dist: " << pair.second.is << std::endl;
    std::cout << "Start at: " << tmpPos.x << ", " << tmpPos.y << ", " << tmpPos.z << std::endl;

    //drawCup(tmpPos);

    validNeighbors = Vector<glm::vec3>();

    // Flag for the enemy is on the ground
    nonGroundCollision = false;

    // Flag to stop update from writing out more than once
    writeOut = false;

    // Greater than 16 to start off the loop
    dirIndex = 0;
    yIndex = 0;
    checkedDirections = (int *)std::calloc(8, sizeof(int));

    // set m_cosCriticalAngle
    m_cosCriticalAngle = std::cos(k_defCriticalAngle);

    // Function called when the object collides with something
    auto collisionCallback([&](const Message & msg_) {
        const CollisionNormMessage & msg(static_cast<const CollisionNormMessage &>(msg_));
        float dot(glm::dot(msg.norm, -SpatialSystem::gravityDir()));
        if (dot >= m_cosCriticalAngle) {
            m_potentialGroundNorm += msg.norm;
        }

        m_groundNorm = Util::safeNorm(m_potentialGroundNorm);
        m_potentialGroundNorm = glm::vec3();

        // if onGround
        if (m_groundNorm != glm::vec3()) {
        	//std::cout << "On ground" << std::endl;

        }
        else {
        	//std::cout << "Not on ground" << std::endl;
        	nonGroundCollision = true;
        }
    });

    Scene::addReceiver<CollisionNormMessage>(&gameObject(), collisionCallback);
}

void MapExploreComponent::update(float dt) {
	int xPos;
	int zPos;
	glm::vec3 testPoint;

	if (oneUpdate || writeOut) {
		glm::vec3 curPos;

		glm::vec3 nextStep;


		curPos = gameObject().getSpatial()->position();

		// While checking collisions and has only a ground collision
		if (collisionCheck && !nonGroundCollision) {
			if (graphSet.find(collisionTestPoint) == graphSet.end()) {
				graphSet.insert(collisionTestPoint);
				//drawCup(collisionTestPoint);
			}
		}
		nonGroundCollision = false;

		// Grid position, the map is contained in a rectangle 97 x 207

		// Second floor height pass
		if (secondFloorPass) {
			if (zIndex < secondFloorDepth) {
				if (xIndex < secondFloorWidth) {
					xPos = xIndex++ + secondFloorStart_x;
					zPos = zIndex + secondFloorStart_z;

					//std::cout << "Pos: " << xPos << ", " << zPos << std::endl;

					auto pair(CollisionSystem::pick(Ray(glm::vec3(xPos, secondFloorHeight, zPos), glm::vec3(0, -1, 0)), &gameObject()));
					if (pair.second.is) {
						testPoint = glm::vec3(xPos, secondFloorHeight - pair.second.dist, zPos);

						if (visitedSet.find(testPoint) == visitedSet.end()) {
							visitedSet.insert(testPoint);
							//`drawCup(testPoint);
						}
					}
				}
				else {
					xIndex = 0;
					zIndex++;
				}
			}
			else {
				std::cout << "Second Floor Done: " << visitedSet.size() << std::endl;
				secondFloorPass = false;
				firstFloorPass = true;
				zIndex = 0;
				xIndex = 0;
			}
		}
		// First floor height pass
		else if (firstFloorPass) {
			if (zIndex < firstFloorDepth) {
				if (xIndex < firstFloorWidth) {
					xPos = xIndex++ + firstFloorStart_x;
					zPos = zIndex + firstFloorStart_z;

					auto pair(CollisionSystem::pick(Ray(glm::vec3(xPos, firstFloorHeight, zPos), glm::vec3(0, -1, 0)), &gameObject()));
					if (pair.second.is) {
						testPoint = glm::vec3(xPos, firstFloorHeight - pair.second.dist, zPos);

						if (visitedSet.find(testPoint) == visitedSet.end()) {
							visitedSet.insert(testPoint);
							//drawCup(testPoint);
						}
					}
				}
				else {
					xIndex = 0;
					zIndex++;
				}
			}
			else {
				std::cout << "First Floor Done: " << visitedSet.size() << std::endl;
				firstFloorPass = false;
				collisionCheck = true;
				visitIterator = visitedSet.begin();
			}
		}
		else if (collisionCheck) {
			if (visitIterator != visitedSet.end()) {
				collisionCount++;
				gameObject().getSpatial()->setPosition(glm::vec3(visitIterator->x, visitIterator->y + .4f, visitIterator->z), false);
				collisionTestPoint = *visitIterator;
				visitIterator++;
			}
			else {
				std::cout << "Collision Check Done: " << graphSet.size() << std::endl;
				std::cout << "Count: " << collisionCount << std::endl;
				collisionCheck = false;
				findNeighbors = true;
			}
		}
		else if (findNeighbors) {
			const int xdir[] = {1, 1, 0, -1, -1, -1, 0, 1};
			const int zdir[] = {0, 1, 1, 1, 0, -1, -1, -1};
			Vector<glm::vec3> possibleNeighbors;
			Vector<glm::vec3> neighbors;

			std::cout << "Removing Outliers" << std::endl;
			removeOutliers(graphSet);
			std::cout << "Size After: " << graphSet.size() << std::endl;

			// iterate through the graphSet and find the neighbors of each position
			for (auto iter = graphSet.begin(); iter != graphSet.end(); ++iter) {
				curPos = *iter;
				neighbors = Vector<glm::vec3>();

				for (int dir = 0; dir < 8; dir++) {
					possibleNeighbors = gridFind(graphSet, iter->x + xdir[dir], iter->z + zdir[dir]);

					//walkways
					if (possibleNeighbors.size() == 2) {
						if (possibleNeighbors[0].y == curPos.y) {

							if (validNeighbor(curPos, possibleNeighbors[0])) {
								neighbors.push_back(possibleNeighbors[0]);
							}
						}
						else {
							if (validNeighbor(curPos, possibleNeighbors[1])) {
								neighbors.push_back(possibleNeighbors[1]);
							}
						}
					}
					else if (possibleNeighbors.size() == 1) {
						if (validNeighbor(curPos, possibleNeighbors[0])) {
							neighbors.push_back(possibleNeighbors[0]);
						}

					}
					else if (possibleNeighbors.size() > 0) {
						std::cout << "Not sure, something is probably wrong" << std::endl;
					}
				}

				// Don't add nodes which aren't connected
				if (neighbors.size() > 2)
					graph.emplace(curPos, neighbors);
			}

			std::cout << "Find Neighbors Done: " << graph.size() << std::endl;
// int loopCount = 0;
		vecvecMap cameFrom = vecvecMap();
// 			for (auto iter = graph.begin(); iter != graph.end();) {
// 				std::cout << "path loop: " << loopCount++ << std::endl;
// 				if (!PathfindingComponent::aStarSearch(graph, glm::vec3(-9, -1.688156, -172), iter->first, cameFrom)) {
// 					iter = graph.erase(iter);
// 				}
// 				else {
// 					++iter;
// 				}

// 			}

			std::cout << "Remove unconnected Nodes: " << graph.size() << std::endl;

			for (auto iter = graph.begin(); iter != graph.end(); ++iter) {
				if (iter->second.size() > 2)
					drawCup(iter->first);
				// for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
				// 	drawCup(*iter2);
				// }
			}

			//-20, 4.05946, -181
			//40, 4.05946, 15
			if (PathfindingComponent::aStarSearch(graph, glm::vec3(-9, -1.688156, -172), glm::vec3(40, 4.05946, 15), cameFrom)) {
				std::cout << "A* found a path between the test points" << std::endl;
			}
			else {
				std::cout << "A* didn't find a path between the test points" << std::endl;
			}

			std::cout << "Write Out" << std::endl;
			writeToFile(graph);
			writeOut = true;

			findNeighbors = false;
		}

	}
	else {
		oneUpdate = true;
	}
}

// Get rid of posistions that shouldn't be part of the map's graph, such as positions on top of the tables
bool MapExploreComponent::removeOutliers(std::unordered_set<glm::vec3, vecHash, gridCompare> &graphSet) {
	// if the posistion doesn't have at least 3 additional nodes in the graph next to it with the same y then get rid of it

	const int xdir[] = {0, 1, 0, -1};
	const int zdir[] = {1, 0, -1, 0};
	int testRadius = 5;
	int count;

	for (auto iter = graphSet.begin(); iter != graphSet.end();) {
		count = 0;
		for (int dir = 0; dir < 4; dir++) {
			for (int i = 0; i < testRadius; i++) {
				//std::cout << "Looking for: " << iter->x + (xdir[dir] * i) << ", " << iter->y << ", " << iter->z + (zdir[dir] * i) << std::endl;
				if (graphSet.find(glm::vec3(iter->x + (xdir[dir] * i), iter->y, iter->z + (zdir[dir] * i))) == graphSet.end()) {
					break;
				}
				
				count++;
			}
		}

		if (count < 6) {
			iter = graphSet.erase(iter);
		}
		else {
			++iter;
		}
	}
}

// Checks if there is something between the two points and if they are close enough
bool MapExploreComponent::validNeighbor(glm::vec3 curPos, glm::vec3 candidate) {
	glm::vec3 direction = Util::safeNorm(curPos - candidate);
	float maxDist = 1.45;
/*
	auto pair(CollisionSystem::pick(Ray(curPos, direction), &gameObject()));
	if (pair.second.is) {
		if (pair.second.dist > glm::distance(curPos, candidate)) {
			// must be close or it is a drop
			if (glm::distance(curPos, candidate) < maxDist || curPos.y > candidate.y) 
				return true;
		}
	}
	else if (glm::distance(curPos, candidate) < maxDist) {
		return true;
	}
	*/

	if (glm::distance(curPos, candidate) < maxDist || curPos.y > candidate.y)
		return true;

	return false;
}

// Find the pos with given x and z coords, could return 0, 1, or 2 because of the raised walkways
Vector<glm::vec3> MapExploreComponent::gridFind(std::unordered_set<glm::vec3, vecHash, gridCompare> &graphSet, int xPos, int zPos) {
	Vector<glm::vec3> found = Vector<glm::vec3>();

	for (auto iter = graphSet.begin(); iter != graphSet.end(); ++iter) {
		if (iter->x == xPos && iter->z == zPos) {
			found.push_back(*iter);
		}
	}

	return found;
}

void MapExploreComponent::writeToFile(vecvectorMap &graph) {
	std::ofstream outFile;

	outFile.open(filename);

	if (outFile) {
		for (auto iter = graph.begin(); iter != graph.end(); ++iter) {
			outFile <<
				iter->first.x << "," <<
				iter->first.y << "," <<
				iter->first.z << "," <<
				vectorToString(iter->second) << '\n';

		}
	}


}

std::string MapExploreComponent::vectorToString(Vector<glm::vec3> vec) {
	std::string ret = "";

	for (glm::vec3 tmpVec : vec) {
		ret += glm::to_string(tmpVec) + ",";
	}

	return ret;
}

void MapExploreComponent::drawCup(glm::vec3 position) {
	Mesh * mesh(Loader::getMesh("Cup.obj"));
    DiffuseShader * shader(RenderSystem::getShader<DiffuseShader>());
    ModelTexture modelTex(.2f, glm::vec3(.20f, 1.0f, .20f), glm::vec3(1.0f));
    bool toon(false);
    glm::vec3 scale(.1f);
    unsigned int collisionWeight(0);
    float moveSpeed(0.0f);

    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, position, scale));
    //DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj, shader->pid, *mesh, modelTex, true, glm::vec2(1, 1)));
    DiffuseRenderComponent & renderComp = Scene::addComponent<DiffuseRenderComponent>(obj, spatComp, shader->pid, *mesh, modelTex, toon, glm::vec2(1,1));
}

