// allows program to be run on dedicated graphics processor for laptops with
// both integrated and dedicated graphics using Nvidia Optimus
#ifdef _WIN32
extern "C" {
    _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
}
#endif



#include <iostream>

#include "glm/gtx/transform.hpp"

#include "EngineApp/EngineApp.hpp"



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



// Constants
const float k_ambience = 0.2f;
const float k_fov = 45.0f;
const float k_near = 0.1f;
const float k_far = 300.0f;
const glm::vec3 k_gravity = glm::vec3(0.0f, -10.0f, 0.0f);

// Player data and fuctions
namespace player {

    const float k_height = 1.75f;
    const float k_width = k_height / 4.0f;
    const float k_lookSpeed = 0.005f;
    const float k_moveSpeed = 5.0f;
    const float k_sprintSpeed = 15.0f;
    const float k_jumpSpeed = 5.0f;

    GameObject * gameObject;
    SpatialComponent * spatialComp;
    NewtonianComponent * newtonianComp;
    CapsuleBounderComponent * bounderComp;
    CameraComponent * cameraComp;
    PlayerControllerComponent * controllerComp;

    void setup(const glm::vec3 & position) {
        gameObject = &Scene::createGameObject();
        spatialComp = &Scene::addComponent<SpatialComponent>(*gameObject, position);
        newtonianComp = &Scene::addComponent<NewtonianComponent>(*gameObject);
        Scene::addComponentAs<GravityComponent, AcceleratorComponent>(*gameObject);
        Scene::addComponent<GroundComponent>(*gameObject);
        Capsule playerCap(glm::vec3(0.0f, -k_height * 0.5f + k_width, 0.0f), k_width, k_height - 2.0f * k_width);
        bounderComp = &Scene::addComponentAs<CapsuleBounderComponent, BounderComponent>(*gameObject, 5, playerCap);
        cameraComp = &Scene::addComponent<CameraComponent>(*gameObject, k_fov, k_near, k_far);
        controllerComp = &Scene::addComponent<PlayerControllerComponent>(*gameObject, k_lookSpeed, k_moveSpeed, k_jumpSpeed, k_sprintSpeed);

        // An example of using object initialization message
        auto initCallback([&](const Message & msg) {            
            cameraComp->lookInDir(cameraComp->getLookDir());
        });
        Scene::addReceiver<ObjectInitMessage>(gameObject, initCallback);
    }

}

// Freecam data and functions
namespace freecam {

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
Vector<GameObject *> f_particleEffects;

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
    DiffuseRenderComponent & renderComp = Scene::addComponent<DiffuseRenderComponent>(obj, shader->pid, *mesh, modelTex, toon);   
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
    DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj, shader->pid, *mesh, modelTex, true));
    ProjectileComponent & projectileComp(Scene::addComponent<ProjectileComponent>(obj));
    
    f_projectiles.push_back(&obj);
}

void createParticleEffect(ParticleEffect::Effect effect, glm::vec3 pos) {
    // Need to create function that maps effects to .objs and .pngs
    Mesh * mesh(Loader::getMesh("Hamburger.obj"));
    ParticleShader * shader(RenderSystem::getShader<ParticleShader>());
    bool toon(true);

    GameObject & obj(Scene::createGameObject());
    ParticleComponent & particleComp(Scene::addComponent<ParticleComponent>(obj));
    ParticleRenderComponent & renderComp(Scene::addComponent<ParticleRenderComponent>(obj, shader->pid, true, effect));
    
    obj.getComponentByType<ParticleComponent>()->spawnParticleEffect(effect, pos);
    f_particleEffects.push_back(&obj);
}

}



int main(int argc, char **argv) {
    if (parseArgs(argc, argv) || EngineApp::init()) {
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }

    //--------------------------------------------------------------------------
    // Shader Setup

    // Diffuse shader
    if (!RenderSystem::createShader<DiffuseShader>("diffuse_vert.glsl", "diffuse_frag.glsl", light::dir)) {
        return EXIT_FAILURE;
    }

    // Bounder shader
    if (!RenderSystem::createShader<BounderShader>("bounder_vert.glsl", "bounder_frag.glsl")) {
        return EXIT_FAILURE;
    }
    
    // Ray shader
    if (!RenderSystem::createShader<RayShader>("ray_vert.glsl", "ray_frag.glsl")) {
        return EXIT_FAILURE;
    }

    if (!RenderSystem::createShader<ParticleShader>("particle_vert.glsl", "particle_frag.glsl", light::dir)) {
        return EXIT_FAILURE;
    }

    //--------------------------------------------------------------------------
    // General setup

    // Disable cursor
    Window::setCursorEnabled(false);

    // Set Gravity
    SpatialSystem::setGravity(k_gravity);

    // Load Level
    Loader::loadLevel(EngineApp::RESOURCE_DIR + "GameLevel_03.json", k_ambience);

    // Setup Player
    player::setup(glm::vec3(0.0f, 6.0f, 0.0f));

    // Setup Free Cam
    freecam::setup();

    // Set primary camera
    RenderSystem::setCamera(player::cameraComp);

    // Set Sound camera
    SoundSystem::setCamera(player::cameraComp);

    // Add Enemies
    int nEnemies(5);
    for (int i(0); i < 5; ++i) {
        createEnemy(glm::vec3(-(nEnemies - 1) * 0.5f + i, 5.0f, -10.0f));
    }

    //--------------------------------------------------------------------------
    // Imgui Panes

#ifdef DEBUG_MODE

    GameObject & imguiGO(Scene::createGameObject());
    
    // Game Stats
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Stats",
        [&]() {
            ImGui::Text("FPS: %d, dt: %f", EngineApp::fps, EngineApp::timeStep);
            ImGui::NewLine();
            ImGui::Text("Workload by System (Update, Messaging)");
            float factor(100.0f / Scene::totalDT);
            ImGui::Text("    Init Queue: %5.2f%%", Scene::initDT * factor);
            ImGui::Text("    Game Logic: %5.2f%%, %5.2f%%", Scene::    gameLogicDT * factor, Scene::    gameLogicMessagingDT * factor);
            ImGui::Text("   Pathfinding: %5.2f%%, %5.2f%%", Scene::  pathfindingDT * factor, Scene::  pathfindingMessagingDT * factor);
            ImGui::Text("       Spatial: %5.2f%%, %5.2f%%", Scene::      spatialDT * factor, Scene::      spatialMessagingDT * factor);
            ImGui::Text("     Collision: %5.2f%%, %5.2f%%", Scene::    collisionDT * factor, Scene::    collisionMessagingDT * factor);
            ImGui::Text("Post Collision: %5.2f%%, %5.2f%%", Scene::postCollisionDT * factor, Scene::postCollisionMessagingDT * factor);
            ImGui::Text("        Render: %5.2f%%, %5.2f%%", Scene::       renderDT * factor, Scene::       renderMessagingDT * factor);
            ImGui::Text("    Kill Queue: %5.2f%%", Scene::killDT * factor);
            ImGui::NewLine();
            ImGui::Text("Player Pos:\n%f %f %f",
                player::spatialComp->position().x,
                player::spatialComp->position().y,
                player::spatialComp->position().z
            );
        }
    );

    // Misc
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Misc",
        [&]() {
            /* Light dir */
            ImGui::SliderFloat3("LightDir", glm::value_ptr(light::dir), -1.f, 1.f);
            /* VSync */
            if (ImGui::Button("VSync")) {
                Window::toggleVSync();
            }
            /* Path finding */
            if (ImGui::Button("Turn off Path finding")) {
                for (auto e : f_enemies) {
                    PathfindingComponent *p = e->getComponentByType<PathfindingComponent>();
                    if (p) {
                        p->setMoveSpeed(0.f);
                    }
                }
            }
        }
    );

    // Toon shading config
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Diffuse Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                RenderSystem::getShader<DiffuseShader>()->toggleEnabled();
            }
            if (ImGui::Button("Wireframe")) {
                RenderSystem::getShader<DiffuseShader>()->toggleWireFrame();
            }
            if (ImGui::Button("Toon")) {
                RenderSystem::getShader<DiffuseShader>()->toggleToon();
            }
            if (RenderSystem::getShader<DiffuseShader>()->isToon()) {
                float angle = RenderSystem::getShader<DiffuseShader>()->getSilAngle();
                ImGui::SliderFloat("Silhouette Angle", &angle, 0.f, 1.f);
                RenderSystem::getShader<DiffuseShader>()->setSilAngle(angle);
                
                int cells = RenderSystem::getShader<DiffuseShader>()->getCells();
                if (ImGui::SliderInt("Cells", &cells, 1, 15)) {
                    RenderSystem::getShader<DiffuseShader>()->setCells(cells);
                }

                /* Make a new pane to define cell values */
                ImGui::End();
                ImGui::Begin("Cell Shading");
                for (int i = 0; i < cells; i++) {
                    float vals[3];
                    float minBounds[3] = { -1.f,  0.f,  0.f };
                    float maxBounds[3] = {  1.f,  1.f,  1.f };
                    vals[0] = RenderSystem::getShader<DiffuseShader>()->getCellIntensity(i);
                    vals[1] = RenderSystem::getShader<DiffuseShader>()->getCellDiffuseScale(i);
                    vals[2] = RenderSystem::getShader<DiffuseShader>()->getCellSpecularScale(i);
                    ImGui::SliderFloat3(("Cell " + std::to_string(i)).c_str(), vals, minBounds, maxBounds);
                    RenderSystem::getShader<DiffuseShader>()->setCellIntensity(i, vals[0]);
                    RenderSystem::getShader<DiffuseShader>()->setCellDiffuseScale(i, vals[1]);
                    RenderSystem::getShader<DiffuseShader>()->setCellSpecularScale(i, vals[2]);
                }
            }
        }
    );

    // Bounder shader toggle
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Bounder Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                RenderSystem::getShader<BounderShader>()->toggleEnabled();
            }
        }
    );

    // Ray shader toggle
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Ray Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                RenderSystem::getShader<RayShader>()->toggleEnabled();
            }
        }
    );

#endif

    //--------------------------------------------------------------------------
    // Message Handling

    // Fire projectile (click)
    // Remove all projectiles (right click)
    auto fireCallback([&](const Message & msg_) {
        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && !msg.mods && msg.action == GLFW_PRESS) {
            createProjectile(player::spatialComp->position() + player::cameraComp->getLookDir() * 2.0f, player::cameraComp->getLookDir());
        }
        if (msg.button == GLFW_MOUSE_BUTTON_2 && msg.action == GLFW_PRESS) {
            for (GameObject * obj : f_projectiles) {
                Scene::destroyGameObject(*obj);
            }
            f_projectiles.clear();
        }
    });
    Scene::addReceiver<MouseMessage>(nullptr, fireCallback);

    // Shoot ray (cntrl-click)
    int rayDepth(100);
    Vector<glm::vec3> rayPositions;
    auto rayPickCallback([&](const Message & msg_) {
        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && msg.mods & GLFW_MOD_CONTROL && msg.action == GLFW_PRESS) {
            rayPositions.clear();
            rayPositions.push_back(player::spatialComp->position());
            glm::vec3 dir(player::cameraComp->getLookDir());
            for (int i(0); i < rayDepth; ++i) {
                auto pair(CollisionSystem::pick(Ray(rayPositions.back(), dir), player::gameObject));
                if (!pair.second.is) {
                    break;
                }
                rayPositions.push_back(pair.second.pos);
                dir = glm::reflect(dir, pair.second.norm);
            }
            RenderSystem::getShader<RayShader>()->setPositions(rayPositions);
        }
    });
    Scene::addReceiver<MouseMessage>(nullptr, rayPickCallback);

    // Toggle Freecam (ctrl-tab)
    auto camSwitchCallback([&](const Message & msg_) {
        static bool free = false;

        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_TAB && msg.action == GLFW_PRESS && msg.mods & GLFW_MOD_CONTROL) {
            if (free) {
                // disable camera controller
                freecam::controllerComp->setEnabled(false);
                // enable player controller
                player::controllerComp->setEnabled(true);
                RenderSystem::setCamera(player::cameraComp);
            }
            else {
                // disable player controller
                player::controllerComp->setEnabled(false);
                // enable camera object
                freecam::controllerComp->setEnabled(true);
                // set camera object camera to player camera
                freecam::spatialComp->setPosition(player::spatialComp->position());
                freecam::spatialComp->setUVW(player::spatialComp->u(), player::spatialComp->v(), player::spatialComp->w());
                freecam::cameraComp->lookInDir(player::cameraComp->getLookDir());
                RenderSystem::setCamera(freecam::cameraComp);
            }
            free = !free;
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, camSwitchCallback);

    // Flip Gravity (ctrl-g)
    auto gravSwapCallback([&](const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_G && msg.action == GLFW_PRESS && msg.mods == GLFW_MOD_CONTROL) {
            SpatialSystem::setGravity(-SpatialSystem::gravity());
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, gravSwapCallback);

    // Destroy game object looking at (delete)
    auto deleteCallback([&] (const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_DELETE && msg.action == GLFW_PRESS) {
            auto pair(CollisionSystem::pick(Ray(player::spatialComp->position(), player::cameraComp->getLookDir()), player::gameObject));
            if (pair.first) Scene::destroyGameObject(pair.first->gameObject());
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, deleteCallback);

    //--------------------------------------------------------------------------

    // Main loop
    EngineApp::run();

    return EXIT_SUCCESS;
}
