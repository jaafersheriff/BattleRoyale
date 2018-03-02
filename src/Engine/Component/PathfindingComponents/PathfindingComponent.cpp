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

    curPosNeighbors = Vector<glm::vec3>();
    //visitedSet = unordered_set<glm::vec3>();
    pos_queue = std::queue<glm::vec3>();

    // Node for the starting pos
    //pos_queue.push(gameObject().getSpatial()->position());

    // Flag for if we recieved a collision message
    prevCollision = false;

    // Greater than 16 to start off the loop
    dirIndex = 100;

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

    float stepSize = 3.0f;
    const int xdir[] = {1, 1, 0, -1, -1, -1, 0, 1};
    const int zdir[] = {0, 1, 1, 1, 0, -1, -1, -1};

    //if (slowTime++ > 1) {

	    // sanity check
	    // should never be have a collision at a pos that is in the visited set
	    auto search = visitedSet.find(gameObject().getSpatial()->position());
	    if (search != visitedSet.end() && prevCollision) {
	    	std::cout << "Collision occured at a position in the vistiedSet: " + glm::to_string(gameObject().getSpatial()->position()) << std::endl;
	    }


	    // The previous move didn't collide with anything, create of get its node and update the graph with it
	    if (!prevCollision && visitedSet.find(gameObject().getSpatial()->position()) == visitedSet.end()) {
	    	//std::cout << "Pushed: " + glm::to_string(gameObject().getSpatial()->position()) << std::endl;
	    	pos_queue.push(gameObject().getSpatial()->position());
	    	//print_queue(pos_queue);

	    	auto ret = visitedSet.insert(gameObject().getSpatial()->position());
	    	if (ret.second) {
	    		glm::vec3 tmp = gameObject().getSpatial()->position();
	    		std::cout << "Added " << tmp.x << ", " << tmp.y << ", " << tmp.z << " to visited" << std::endl;

	    		glm::vec3 position = gameObject().getSpatial()->position();
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
	    }

	    // Reset the collision flag
	    prevCollision = false;

	    // while the object hasn't tried every direction
	    if (dirIndex < 16) {
	    	if (dirIndex % 2) {
	    		gameObject().getSpatial()->setPosition(glm::vec3(curPos.x, curPos.y, curPos.z), false);
	    	}
	    	else {
	    		//std::cout << "x: " << xdir[dirIndex / 2] << " z: " << zdir[dirIndex/2] << std::endl;
	    		// every other tick is a move out from the center, divide by 2 to get the actual direction index
	    		int index = dirIndex / 2;
	    		// Cardinal directions
	    		if (!xdir[index] || !zdir[index]) {
	    			gameObject().getSpatial()->move(Util::safeNorm(glm::vec3(xdir[index], 0, zdir[index])) * stepSize);
	    		}
	    		// Diagonals, multiply by sqrt(2) to make a grid and not a circle
	    		else {
	    			gameObject().getSpatial()->move(Util::safeNorm(glm::vec3(xdir[index], 0, zdir[index])) * stepSize * (float)sqrt(2));
	    		}
	    	}
	    	dirIndex++;
	    }
	    else {
	    	// Attempted to move to every neighbor
	    	dirIndex = 0;
	    	curPos = pos_queue.front();
	    	pos_queue.pop();

	    //	std::cout << "new curPos: " + glm::to_string(curPos) << std::endl;

	    }
	    //std::cout << gameObject().getSpatial()->position().x << ", " << gameObject().getSpatial()->position().y << ", " << gameObject().getSpatial()->position().z << std::endl;
	    //slowTime = 0;
	//}

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