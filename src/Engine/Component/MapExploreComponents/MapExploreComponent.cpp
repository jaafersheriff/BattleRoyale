#include "MapExploreComponent.hpp"

#include "Scene/Scene.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "System/MapExploreSystem.hpp"
#include "System/CollisionSystem.hpp"

MapExploreComponent::MapExploreComponent(GameObject & gameObject, float ms) :
	Component(gameObject),
	m_spatial(nullptr),
	m_moveSpeed(ms)
{}

void MapExploreComponent::init() {

    if (!(m_spatial = gameObject().getSpatial())) assert(false);

    slowTime = 0;

    visitedSet = std::unordered_set<glm::vec3, vecHash, customVecCompare>();
    //visitedSet = Vector<glm::vec3>();
    pos_queue = std::queue<glm::vec3>();
    //curPos = m_spatial->position();
    //std::cout << "curPos: " << curPos.x << ", " << curPos.y << ", " << curPos.z << std::endl;

    //auto pair(CollisionSystem::pick(Ray(curPos, glm::vec3(0,-1,0)), &gameObject()));

    glm::vec3 tmpPos = gameObject().getSpatial()->position();
    //tmpPos = glm::vec3(tmpPos.x, tmpPos.y - pair.second.dist, tmpPos.z);

    // Node for the starting pos
    visitedSet.insert(tmpPos);

    // Set curPos to the objects initial pos
    searchFromPos = tmpPos;
    //std::cout << "dist: " << pair.second.is << std::endl;
    std::cout << "Start at: " << tmpPos.x << ", " << tmpPos.y << ", " << tmpPos.z << std::endl;

    //drawCup(tmpPos);

    validNeighbors = Vector<glm::vec3>();

    // Flag for the enemy is on the ground
    nonGroundCollision = false;

    // Flag to stop update from writing out more than once
    writeOut = true;

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
	// let everything have at least one update
	if (oneUpdate) {
		const int xdir[] = {1, 1, 0, -1, -1, -1, 0, 1};
		const int zdir[] = {0, 1, 1, 1, 0, -1, -1, -1};
		const float ydir[] = {0, .55f};
		float stepSize = 1.f;
		float extension = 0.f;
		glm::vec3 curPos;
		float distance = 0.f;


	if (slowTime++ > 1) {

		curPos = gameObject().getSpatial()->position();

		if (!nonGroundCollision && curPos != searchFromPos) {

			auto pair(CollisionSystem::pick(Ray(curPos, glm::vec3(0,-1,0)), &gameObject()));
	        if (pair.second.is) {       

	        	curPos = glm::vec3(curPos.x, curPos.y - pair.second.dist, curPos.z); 
				
				//std::cout << "CurPos: " << curPos.x << ", " << curPos.y << ", " << curPos.z << std::endl;

				// curPos isn't in the visitedSet
	        	if (visitedSet.find(curPos) == visitedSet.end()) {
					// Add curPos to the visitedSet and push it onto the queue to be searched
					visitedSet.insert(curPos);
					pos_queue.push(curPos);

					// add to searchFrom's validNeighbors
					validNeighbors.push_back(curPos);
					//std::cout << "added neighbor" << std::endl;
					checkedDirections[(dirIndex - 1) / 2] = 1;

					//std::cout << "Added: " << curPos.x << ", " << curPos.y << ", " << curPos.z << std::endl;

					//DEBUG
					drawCup(curPos);
				}
			}
		}
	
		nonGroundCollision = false;

		if (yIndex < 2) {
			if (dirIndex < 16) {
				if (dirIndex % 2) {
					gameObject().getSpatial()->setPosition(searchFromPos, false);
				}
				else {
					// every other ticke is a move out from the center, divide by 2 to get the correct dirIndex
					int index = dirIndex / 2;
					if (!checkedDirections[index]) {

						// ydir == 0
						if (ydir[yIndex] == 0) {
							// Corner
							if (xdir[index] && zdir[index]) {
								//std::cout << "Corner" << std::endl;
								distance = sqrt(2) * stepSize;
							}
							// Cardinal
							else {
								//std::cout << "Cardinal" << std::endl;
								distance = stepSize;
							}
						}
						else {
							// Corner
							if (xdir[index] && zdir[index]) {
								//std::cout << "Top Corner" << std::endl;
								distance = sqrt(pow(ydir[yIndex], 2) + 2 * stepSize * stepSize);
							}
							// Cardinal
							else {
								//std::cout << "Top Cardinal" << std::endl;
								distance = sqrt(stepSize * stepSize + pow(ydir[yIndex], 2));
							}
						}

						glm::vec3 dirStep = Util::safeNorm(glm::vec3(xdir[index], ydir[yIndex], zdir[index])) * distance;
						glm::vec3 nextStep = searchFromPos + dirStep;
						auto pair(CollisionSystem::pick(Ray(nextStep, glm::vec3(0,-1,0)), &gameObject()));
		        		if (pair.second.is) {  

		        			nextStep = glm::vec3(nextStep.x, nextStep.y - pair.second.dist, nextStep.z);

							// If the point has been visited before, we don't need to check it again, can just add it and move on
							if (visitedSet.find(nextStep) ==  visitedSet.end()) {
								//std::cout << "new node" << std::endl;
								//std::cout << "Move to: " << nextStep.x << ", " << nextStep.y << ", " << nextStep.z << std::endl;
								gameObject().getSpatial()->move(dirStep);
							}
							else {
								//glm::vec3 closest = closestPos(nextStep);
								//if (!checkedDirections[index]) {
									validNeighbors.push_back(nextStep);
									//std::cout << "added neighbor" << std::endl;
									checkedDirections[index] = 1;
								//}
								dirIndex++;
							}
						}
					}
				}
				dirIndex++;
			}
			// Searched all neighbors, move to a new searchFromPos
			else {
				dirIndex = 0;
				yIndex++;
			}
		}
		else {
			yIndex = 0;

			if (validNeighbors.size() > 8) {
				std::cout << "To many neighbors" << std::endl;
			}

			std::fill_n(checkedDirections, 8, 0);
			// create node and push it in to graph
			graph.emplace(searchFromPos, validNeighbors);
			validNeighbors = Vector<glm::vec3>();

			if (!pos_queue.empty() && nodeCount < 640) {

				std::cout << "Count: " << nodeCount++ << std::endl;
				searchFromPos = pos_queue.front();
				pos_queue.pop();
			}
			else if (writeOut) {
				writeOut = false;
				std::cout << "Writing Out" << std::endl;
				std::ofstream outFile;
				outFile.open("testOut.txt");

				if (outFile) {
					for (auto iter = graph.begin(); iter != graph.end(); ++iter) {
						outFile <<
							iter->first.x << "," <<
							iter->first.y << "," <<
							iter->first.z << "," <<
							vectorToString(iter->second) << '\n';

					}
				}

				std::cout << "Length of total first floor graph: " << graph.size() << std::endl;
				searchFromPos = glm::vec3(0.f);
			}

			gameObject().getSpatial()->setPosition(searchFromPos, false);

		}

	slowTime = 0;

	}	
	}
	else {
		oneUpdate = true;
	}

}

void MapExploreComponent::writeToFile(String filename, vecvectorMap &graph) {
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

