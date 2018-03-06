#include "PathfindingComponent.hpp"

#include "glm/gtx/norm.hpp"

#include "System/PathfindingSystem.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Scene/Scene.hpp"
#include "Util/Util.hpp"

#include "glm/gtx/string_cast.hpp"


PathfindingComponent::PathfindingComponent(GameObject & gameObject, GameObject & player, float ms) :
    Component(gameObject),
    m_spatial(nullptr),
    m_player(&player),
    m_moveSpeed(ms)
{}

void PathfindingComponent::init() {

    if (!(m_spatial = gameObject().getSpatial())) assert(false);

    slowTime = 0;

    //visitedSet = std::unordered_set<glm::vec3, vecHash, customCompare>();
    visitedSet = Vector<glm::vec3>();
    pos_queue = std::queue<glm::vec3>();

    glm::vec3 tmpPos = gameObject().getSpatial()->position();

    // Node for the starting pos
    visitedSet.push_back(tmpPos);

    // Set curPos to the objects initial pos
    searchFromPos = tmpPos;

    std::cout << "Start at: " << tmpPos.x << ", " << tmpPos.y << ", " << tmpPos.z << std::endl;

    //drawCup(tmpPos);

    validNeighbors = Vector<glm::vec3>();

    // Flag for if we recieved a collision message
    prevCollision = false;

    // Greater than 16 to start off the loop
    dirIndex = 0;

    // Function called when the object collides with something
    auto collisionCallback([&](const Message & msg_) {
    	const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));

    	//std::cout << "Radius: " << msg.bounder1.enclosingSphere().radius  << std::endl;

    	// The last move caused a collision
    	prevCollision = true;

    });


    Scene::addReceiver<CollisionMessage>(&gameObject(), collisionCallback);
}

void PathfindingComponent::update(float dt) {

	const int xdir[] = {1, 1, 0, -1, -1, -1, 0, 1};
	const int zdir[] = {0, 1, 1, 1, 0, -1, -1, -1};
	float stepSize = 1.f;
	glm::vec3 curPos;

	//if (slowTime++ > 50) {



	curPos = gameObject().getSpatial()->position();

	if (!prevCollision && curPos != searchFromPos) {
		// add to searchFrom's validNeighbors
		validNeighbors.push_back(curPos);
		
		// curPos isn't in the visitedSet
		if (!findInVisited(curPos)) {
			// Add curPos to the visitedSet and push it onto the queue to be searched
			visitedSet.push_back(curPos);
			pos_queue.push(curPos);

			//DEBUG
			drawCup(curPos);
		}
	}
	

	// Reset collision flag
	prevCollision = false;

	if (dirIndex < 16) {
		if (dirIndex % 2) {
			gameObject().getSpatial()->setPosition(searchFromPos, false);
		}
		else {
			// every other ticke is a move out from the center, divide by 2 to get the correct dirIndex
			int index = dirIndex / 2;

			glm::vec3 nextStep = glm::vec3(searchFromPos.x + xdir[index] * stepSize, searchFromPos.y, searchFromPos.z + zdir[index] * stepSize);

			// Cardinal directions
			if (!xdir[index] || !zdir[index]) {
				// If the pos is in the vistitedSet then its valid and we don't need to check collisions
				//if (!findInVisited(glm::vec3(searchFromPos.x + xdir[index], searchFromPos.y, searchFromPos.z + zdir[index]))) {
				if (!findInVisited(nextStep)) {
					gameObject().getSpatial()->move(Util::safeNorm(glm::vec3(xdir[index], 0, zdir[index])) * stepSize);
				}
				else {
					validNeighbors.push_back(nextStep);
				}
			}
			// Diagonals, multiply by sqrt(2) to make a grid and not a circle
			else {
				if (!findInVisited(nextStep)) {
				//if (!findInVisited(glm::vec3(searchFromPos.x + xdir[index], searchFromPos.y, searchFromPos.z + zdir[index]))) {
					gameObject().getSpatial()->move(Util::safeNorm(glm::vec3(xdir[index], 0, zdir[index])) *stepSize * (float)sqrt(2));
				}
				else {
					validNeighbors.push_back(nextStep);
				}
			}
		}
		dirIndex++;
	}
	// Searched all neighbors, move to a new searchFromPos
	else {
		dirIndex = 0;
/*
		std::cout << "**************************" << std::endl;
		std::cout << "Length of neigbor list " << validNeighbors.size() << std::endl;

		for (glm::vec3 vec : validNeighbors) {
			std:: cout << vec.x << ", " << vec.y << ", " << vec.z << std::endl;
		}

		std::cout << "**************************" << std::endl;
*/
		// create node and push it in to graph
		graph.push_back(Node(searchFromPos, validNeighbors));
		validNeighbors = Vector<glm::vec3>();

		if (!pos_queue.empty()) {
		
			searchFromPos = pos_queue.front();
			pos_queue.pop();
		}
		else {
			std::ofstream outFile;
			outFile.open("testOut.txt");

			if (outFile) {
				for (Node node : graph) {
					outFile <<
						node.position.x << "," <<
						node.position.y << "," <<
						node.position.z << "," <<
						vectorToString(node.neighbors) << '\n';

				}
			}

			std::cout << "Length of total first floor graph: " << graph.size() << std::endl;
			searchFromPos = glm::vec3(0.f);
		}

		gameObject().getSpatial()->setPosition(searchFromPos, false);

	}

	slowTime = 0;

	//}	

}

std::string PathfindingComponent::vectorToString(Vector<glm::vec3> vec) {
	std::string ret = "";

	for (glm::vec3 tmpVec : vec) {
		ret += glm::to_string(tmpVec) + ",";
	}

	return ret;
}



bool PathfindingComponent::findInVisited(glm::vec3 vec) {
	float epsilon = .3f;

	//for (glm::vec3 test : visitedSet) {
	for (std::vector<glm::vec3>::reverse_iterator test = visitedSet.rbegin(); test != visitedSet.rend(); ++test) {
		if (abs(test->x - vec.x) < epsilon) {
			if (abs(test->y - vec.y) < epsilon) {
				if (abs(test->z - vec.z) < epsilon) {
					return true;
				}
			}
		}
	}

	return false;
}

void PathfindingComponent::print_queue(std::queue<glm::vec3> q)
{
  while (!q.empty())
  {
    std::cout << glm::to_string(q.front()) << " ";
    q.pop();
  }
  std::cout << std::endl;
}

void PathfindingComponent::drawCup(glm::vec3 position) {
	Mesh * mesh(Loader::getMesh("Cup.obj"));
    DiffuseShader * shader(RenderSystem::getShader<DiffuseShader>());
    ModelTexture modelTex(.2f, glm::vec3(.20f, 1.0f, .20f), glm::vec3(1.0f));
    bool toon(false);
    glm::vec3 scale(.1f);
    unsigned int collisionWeight(0);
    float moveSpeed(0.0f);

    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, position, scale));
    DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj, shader->pid, *mesh, modelTex, true));
}