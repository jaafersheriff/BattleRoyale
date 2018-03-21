#include "MapExploreComponent.hpp"

#include "Scene/Scene.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "System/MapExploreSystem.hpp"
#include "System/CollisionSystem.hpp"

#include "glm/ext.hpp"

#include "Component/Components.hpp"
#include "System/GameSystem.hpp"

MapExploreComponent::MapExploreComponent(GameObject & gameObject, float ms, String filename) :
	Component(gameObject),
	m_spatial(nullptr),
	m_moveSpeed(ms),
	filename(filename)
{}

void MapExploreComponent::init() {

    if (!(m_spatial = gameObject().getSpatial())) assert(false);

    slowTime = 0;

    visitedSet = std::unordered_set<glm::vec3, PathfindingSystem::vecHash, PathfindingSystem::gridCompare>();
    graphSet = std::unordered_set<glm::vec3, PathfindingSystem::vecHash, PathfindingSystem::gridCompare>();

    pos_queue = std::queue<glm::vec3>();
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
        	nonGroundCollision = true;
        }

        //nonGroundCollision = true;

    });

    Scene::addReceiver<CollisionNormMessage>(&gameObject(), collisionCallback);
}

void MapExploreComponent::update(float dt) {
	int xPos;
	int zPos;
	glm::vec3 testPoint;

	if (oneUpdate || writeOut) {

	//if (slowTime++ > 50) {
		glm::vec3 curPos;

		glm::vec3 nextStep;


		curPos = gameObject().getSpatial()->position();

		// While checking collisions and has only a ground collision
		if (collisionCheck && !nonGroundCollision) {
			if (graphSet.find(collisionTestPoint) == graphSet.end()) {
				//collisionTestPoint.y += .25f;
				graphSet.insert(collisionTestPoint);
				//std::cout << "Cup" << std::endl;
				//drawCup(collisionTestPoint, 2);
			}
		}
		nonGroundCollision = false;

		// Grid position, the map is contained in a rectangle 97 x 207

		// Second floor height pass
		if (secondFloorPass) {
			for (int zIndex = 0; zIndex < secondFloorDepth; zIndex += stepSize) {
				for (int xIndex = 0; xIndex < secondFloorWidth; xIndex += stepSize) {
					xPos = xIndex + secondFloorStart_x;
					zPos = zIndex + secondFloorStart_z;

					//std::cout << "Pos: " << xPos << ", " << zPos << std::endl;

					auto pair(CollisionSystem::pickHeavy(Ray(glm::vec3(xPos, secondFloorHeight, zPos), glm::vec3(0, -1, 0)), UINT_MAX));
					if (pair.second.is) {
						testPoint = glm::vec3(xPos, secondFloorHeight - pair.second.dist, zPos);

						if (visitedSet.find(testPoint) == visitedSet.end()) {
							visitedSet.insert(testPoint);
							//drawCup(testPoint);
						}
					}
				}

			}
			std::cout << "Second Floor Done: " << visitedSet.size() << std::endl;
			secondFloorPass = false;
			firstFloorPass = true;

		}
		// First floor height pass
		else if (firstFloorPass) {
			for (int zIndex = 0; zIndex < firstFloorDepth; zIndex += stepSize) {
				for (int xIndex = 0; xIndex < firstFloorWidth; xIndex += stepSize) {
					xPos = xIndex + firstFloorStart_x;
					zPos = zIndex + firstFloorStart_z;

					auto pair(CollisionSystem::pickHeavy(Ray(glm::vec3(xPos, firstFloorHeight, zPos), glm::vec3(0, -1, 0)), UINT_MAX));
					if (pair.second.is) {
						testPoint = glm::vec3(xPos, firstFloorHeight - pair.second.dist, zPos);

						if (visitedSet.find(testPoint) == visitedSet.end()) {
							visitedSet.insert(testPoint);
							//drawCup(testPoint);
						}
					}
				}
			}
			std::cout << "First Floor Done: " << visitedSet.size() << std::endl;
			firstFloorPass = false;
			//collisionCheck = true;
			findNeighbors = true;
			visitIterator = visitedSet.begin();
			graphSet = visitedSet;
		}
		// else if (collisionCheck) {
		// 	if (visitIterator != visitedSet.end()) {
		// 		collisionCount++;
		// 		float offset = m_spatial->position().y - gameObject().getComponentByType<BounderComponent>()->groundPosition().y;
		// 		gameObject().getSpatial()->setRelativePosition(glm::vec3(visitIterator->x, visitIterator->y + offset + .5f, visitIterator->z), false);
		// 		collisionTestPoint = *visitIterator;
		// 		visitIterator++;
		// 	}
		// 	else {
		// 		std::cout << "Collision Check Done: " << graphSet.size() << std::endl;
		// 		std::cout << "Count: " << collisionCount << std::endl;
		// 		collisionCheck = false;
		// 		findNeighbors = true;
		// 	}
		// }
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
					possibleNeighbors = gridFind(graphSet, iter->x + (xdir[dir] * stepSize), iter->z + (zdir[dir] * stepSize));

					//walkways
					if (possibleNeighbors.size() == 2) {
						if (possibleNeighbors[0].y == curPos.y) {

							if (validNeighbor(curPos, possibleNeighbors[0], stepSize)) {
								neighbors.push_back(possibleNeighbors[0]);
							}
						}
						else {
							if (validNeighbor(curPos, possibleNeighbors[1], stepSize)) {
								neighbors.push_back(possibleNeighbors[1]);
							}
						}
					}
					else if (possibleNeighbors.size() == 1) {
						if (validNeighbor(curPos, possibleNeighbors[0], stepSize)) {
							neighbors.push_back(possibleNeighbors[0]);
						}

					}
					else if (possibleNeighbors.size() > 0) {
						std::cout << "Not sure, something is probably wrong" << std::endl;
					}
				}

				// Don't add nodes which aren't connected
				if (neighbors.size() > 4)
					graph.emplace(curPos, neighbors);
			}

			std::cout << "Find Neighbors Done: " << graph.size() << std::endl;

			 for (auto iter = graph.begin(); iter != graph.end(); ++iter) {
			 	if (iter->second.size() == 8) {
			 		drawCup(iter->first);
			 	}
			 	else {
			 		std::cout << "why were you added" << std::endl;
			 	}
			 }
// int loopCount = 0;
		PathfindingSystem::vecvecMap cameFrom = PathfindingSystem::vecvecMap();
// 			for (auto iter = graph.begin(); iter != graph.end();) {
// 				std::cout << "path loop: " << loopCount++ << std::endl;
// 				if (!PathfindingComponent::aStarSearch(graph, glm::vec3(-9, -1.688156, -172), iter->first, cameFrom)) {
// 					iter = graph.erase(iter);
// 				}
// 				else {
// 					++iter;
// 				}

// 			}

			//std::cout << "Remove unconnected Nodes: " << graph.size() << std::endl;

			// for (auto iter = graph.begin(); iter != graph.end(); ++iter) {
			// 	//if (iter->second.size() > 4)
			// 		//drawCup(iter->first);
			// 	 for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
			// 	 	if (iter2->y > iter->first.y)
			// 	 		drawCup(*iter2);
			// 	 }
			// }

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
		slowTime = 0;
	//}

	}
	else {
		oneUpdate = true;
	}
}

// Get rid of posistions that shouldn't be part of the map's graph, such as positions on top of the tables
bool MapExploreComponent::removeOutliers(std::unordered_set<glm::vec3, PathfindingSystem::vecHash, PathfindingSystem::gridCompare> &graphSet) {
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
bool MapExploreComponent::validNeighbor(glm::vec3 curPos, glm::vec3 candidate, float stepSize) {
	//glm::vec3 direction = Util::safeNorm(curPos - candidate);

	// Check that it is a neighbor on the grid
	stepSize += .2;
	if (abs(curPos.x - candidate.x) < stepSize && abs(curPos.z - candidate.z) < stepSize) {
		// if step up or drop down
		if (abs(candidate.y - curPos.y) < .5f) {//} || curPos.y > candidate.y) {
			return true;
		}
	}

	//if (glm::distance(curPos, candidate) < maxDist || curPos.y > candidate.y)
	//	return true;

	return false;
}

// Find the pos with given x and z coords, could return 0, 1, or 2 because of the raised walkways
Vector<glm::vec3> MapExploreComponent::gridFind(std::unordered_set<glm::vec3, PathfindingSystem::vecHash, PathfindingSystem::gridCompare> &graphSet, int xPos, int zPos) {
	Vector<glm::vec3> found = Vector<glm::vec3>();

	for (auto iter = graphSet.begin(); iter != graphSet.end(); ++iter) {
		if (iter->x == xPos && iter->z == zPos) {
			found.push_back(*iter);
		}
	}

	return found;
}

void MapExploreComponent::writeToFile(PathfindingSystem::vecvectorMap &graph) {
	std::ofstream outFile;

	outFile.open(convert(filename));

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

void MapExploreComponent::drawCup(glm::vec3 position, int heightoffset) {
	const Mesh * mesh(Loader::getMesh("Hamburger.obj"));
    const Texture * texture(Loader::getTexture("Hamburger_BaseColor.png"));
    const DiffuseShader * shader();
    ModelTexture modelTex(texture, Material(glm::vec3(1.f), glm::vec3(1.f), 16.0f));
    GameObject & obj(Scene::createGameObject());
    position.y += heightoffset;
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, position, glm::vec3(.05)));
	DiffuseRenderComponent & renderComp = Scene::addComponent<DiffuseRenderComponent>(
        obj,
        spatComp,
        *mesh,
        modelTex,
        false,
        glm::vec2(1.0f),
        false
    );
}






