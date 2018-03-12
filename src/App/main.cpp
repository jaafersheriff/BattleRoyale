// allows program to be run on dedicated graphics processor for laptops with
// both integrated and dedicated graphics using Nvidia Optimus
#ifdef _WIN32
extern "C" {
    _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
}
#endif



#include <iostream>

#include "glm/gtx/transform.hpp"

#include "Game.hpp"



namespace {



void printUsage() {
    std::cout << "Valid arguments: " << std::endl;

    std::cout << "\t-h\n\t\tPrint help" << std::endl;
    
    std::cout << "\t-v\n\t\tSet verbose nature logging" << std::endl;

    std::cout << "\t-r <resource_dir>" << std::endl;
    std::cout << "\t\tSet the resource directory" << std::endl;

    std::cout << "\t-n <application_name>" << std::endl;
    std::cout << "\t\tSet the application name" << std::endl;
}

int parseArgs(int argc, char **argv) {
    /* Process cmd line args */
    for (int i = 0; i < argc; i++) {
        /* Help */
        if (!strcmp(argv[i], "-h")) {
            printUsage();
            return 1;
        }
        /* Verbose */
        if (!strcmp(argv[i], "-v")) {
            EngineApp::verbose = true;
        }
        /* Set resource dir */
        if (!strcmp(argv[i], "-r")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            EngineApp::RESOURCE_DIR = argv[i + 1];
        }
        /* Set application name */
        if (!strcmp(argv[i], "-n")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            EngineApp::APP_NAME = argv[i + 1];
        }
    }
    return 0;
}



}



    const float k_lookSpeed = player::k_lookSpeed;
    const float k_minMoveSpeed = player::k_moveSpeed;
    const float k_maxMoveSpeed = 10.0f * k_minMoveSpeed;

    GameObject * gameObject;
    SpatialComponent * spatialComp;
    CameraComponent * cameraComp;
    CameraControllerComponent * controllerComp;

    void setup() {
        gameObject = &Scene::createGameObject();
        spatialComp = &Scene::addComponent<SpatialComponent>(*gameObject);
        cameraComp = &Scene::addComponent<CameraComponent>(*gameObject, k_fov, k_near, k_far);
        controllerComp = &Scene::addComponent<CameraControllerComponent>(*gameObject, k_lookSpeed, k_minMoveSpeed, k_maxMoveSpeed);
        controllerComp->setEnabled(false);
    }

}

// Light data and functions
namespace light {

    glm::vec3 dir = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));

}

Vector<GameObject *> f_enemies;
Vector<GameObject *> f_projectiles;

void createEnemy(const glm::vec3 & position) {    
    Mesh * mesh(Loader::getMesh("bunny.obj"));
    DiffuseShader * shader(RenderSystem::getShader<DiffuseShader>());
    ModelTexture modelTex(k_ambience, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f));
    bool toon(true);
    glm::vec3 scale(0.75f);
    unsigned int collisionWeight(5);
    float moveSpeed(3.0f);

    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, position, scale));
    NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj));
    GravityComponent & gravComp(Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj));
    BounderComponent & boundComp(CollisionSystem::addBounderFromMesh(obj, collisionWeight, *mesh, false, true, false));
    PathfindingComponent & pathComp(Scene::addComponent<PathfindingComponent>(obj, *player::gameObject, moveSpeed, false));
    DiffuseRenderComponent & renderComp = Scene::addComponent<DiffuseRenderComponent>(obj, spatComp, shader->pid, *mesh, modelTex, toon, glm::vec2(1,1));   
    EnemyComponent & enemyComp(Scene::addComponent<EnemyComponent>(obj));
    
    f_enemies.push_back(&obj);
}

void createProjectile(const glm::vec3 & initPos, const glm::vec3 & dir) {
    Mesh * mesh(Loader::getMesh("Hamburger.obj"));
    DiffuseShader * shader(RenderSystem::getShader<DiffuseShader>());
    Texture * tex(Loader::getTexture("Hamburger_BaseColor.png"));
    ModelTexture modelTex(tex, k_ambience, glm::vec3(1.0f), glm::vec3(1.0f));
    bool toon(true);
    glm::vec3 scale(0.05f);
    unsigned int collisionWeight(5);
    float speed(50.0f);

    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, initPos, scale));
    BounderComponent & bounderComp(CollisionSystem::addBounderFromMesh(obj, collisionWeight, *mesh, false, true, false));
    NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj));
    Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj);
    newtComp.addVelocity(dir * speed);
    DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj, spatComp, shader->pid, *mesh, modelTex, true, glm::vec2(1,1)));
    ProjectileComponent & projectileComp(Scene::addComponent<ProjectileComponent>(obj));
    
    f_projectiles.push_back(&obj);
}



}



int main(int argc, char **argv) {
    if (parseArgs(argc, argv) || EngineApp::init()) {
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }

    startGame();

    return EXIT_SUCCESS;
}
