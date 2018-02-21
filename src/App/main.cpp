// allows program to be run on dedicated graphics processor for laptops with
// both integrated and dedicated graphics using Nvidia Optimus
#ifdef _WIN32
extern "C" {
    _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
}
#endif

#include <string>
#include <iostream>

#include "glm/gtx/transform.hpp"

#include "EngineApp/EngineApp.hpp"
#include "System/GameLogicSystem.hpp"
#include "System/SpatialSystem.hpp"
#include "System/PathfindingSystem.hpp"
#include "System/CollisionSystem.hpp"
#include "System/RenderSystem.hpp"
#include "LevelBuilder/FileReader.hpp"

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

int main(int argc, char **argv) {
    if (parseArgs(argc, argv) || EngineApp::init()) {
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }

    Window::setCursorEnabled(false);

    GameObject & imguiGO(Scene::createGameObject());

    /* Create diffuse shader */
    glm::vec3 lightPos(100.f, 100.f, 100.f);
    // TODO : user shouldn't need to specify resource dir here
    if (!RenderSystem::createShader<DiffuseShader>(
            EngineApp::RESOURCE_DIR + "diffuse_vert.glsl",    /* Vertex shader file       */
            EngineApp::RESOURCE_DIR + "diffuse_frag.glsl",    /* Fragment shader file     */
            lightPos                                      /* Shader-specific uniforms */
        )) {
        std::cerr << "Failed to add diffuse shader" << std::endl;
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }
    /* Diffuse Shader ImGui Pane */
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
        }
    );

    // Create collider
    // alternate method using unique_ptr and new
    if (!RenderSystem::addShader(std::unique_ptr<BounderShader>(new BounderShader(
            EngineApp::RESOURCE_DIR + "bounder_vert.glsl",
            EngineApp::RESOURCE_DIR + "bounder_frag.glsl"
        )))) {
        std::cerr << "Failed to add collider shader" << std::endl;
        std::cin.get(); //don't immediately close the console
        return EXIT_FAILURE;
    }
    /* Collider ImGui pane */
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Bounder Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                RenderSystem::getShader<BounderShader>()->toggleEnabled();
            }
        }
    );

    /* Set Gravity */
    SpatialSystem::setGravity(glm::vec3(0.0f, -10.0f, 0.0f));

    /* Setup Camera */
    float freeCamFOV(45.0f);
    float freeCamLookSpeed(0.2f);
    float freeCamMoveSpeed(5.0f);
    GameObject & freeCam(Scene::createGameObject());
    SpatialComponent & freeCamSpatComp(Scene::addComponent<SpatialComponent>(freeCam));
    CameraComponent & freeCamCamComp(Scene::addComponent<CameraComponent>(freeCam, freeCamFOV));
    CameraControllerComponent & freeCamContComp(Scene::addComponent<CameraControllerComponent>(freeCam, freeCamLookSpeed, freeCamMoveSpeed));
    freeCamContComp.setEnabled(false);

    /* Setup Player */
    float playerHeight(1.75f);
    float playerWidth(playerHeight / 4.0f);
    glm::vec3 playerPos(0.0f, 6.0f, 0.0f);
    float playerFOV(freeCamFOV);
    float playerLookSpeed(freeCamLookSpeed);
    float playerMoveSpeed(freeCamMoveSpeed);
    float playerJumpSpeed(5.0f);
    float playerMaxSpeed(50.0f); // terminal velocity
    GameObject & player(Scene::createGameObject());
    SpatialComponent & playerSpatComp(Scene::addComponent<SpatialComponent>(player));
    playerSpatComp.setPosition(playerPos);
    NewtonianComponent & playerNewtComp(Scene::addComponent<NewtonianComponent>(player, playerMaxSpeed));
    GravityComponent & playerGravComp(Scene::addComponent<GravityComponent>(player));
    GroundComponent & playerGroundComp(Scene::addComponent<GroundComponent>(player));
    Capsule playerCap(glm::vec3(), playerHeight - 2.0f * playerWidth, playerWidth);
    CapsuleBounderComponent & playerBoundComp(Scene::addComponent<CapsuleBounderComponent>(player, 1, playerCap));
    CameraComponent & playerCamComp(Scene::addComponent<CameraComponent>(player, playerFOV));
    PlayerControllerComponent & playerContComp(Scene::addComponent<PlayerControllerComponent>(player, playerLookSpeed, playerMoveSpeed, playerJumpSpeed));

    RenderSystem::setCamera(&playerCamComp);

    // Toggle free camera (ctrl-tab)
    auto camSwitchCallback([&](const Message & msg_) {
        static bool free = false;

        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_TAB && msg.action == GLFW_PRESS && msg.mods & GLFW_MOD_CONTROL) {
            if (free) {
                // disable camera controller
                freeCamContComp.setEnabled(false);
                // enable player controller
                playerContComp.setEnabled(true);
                RenderSystem::setCamera(&playerCamComp);
            }
            else {
                // disable player controller
                playerContComp.setEnabled(false);
                // enable camera object
                freeCamContComp.setEnabled(true);
                // set camera object camera to player camera
                freeCamSpatComp.setPosition(playerSpatComp.position());
                freeCamSpatComp.setUVW(playerSpatComp.u(), playerSpatComp.v(), playerSpatComp.w());
                freeCamCamComp.lookInDir(playerCamComp.getLookDir());
                RenderSystem::setCamera(&freeCamCamComp);
            }
            free = !free;
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, camSwitchCallback);

    // Demo ray picking (click)
    auto rayPickCallback([&](const Message & msg_) {
        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && msg.action == GLFW_PRESS) {
            auto pair(CollisionSystem::pick(Ray(playerSpatComp.position(), playerCamComp.getLookDir())));
            if (pair.first && pair.first->weight() < UINT_MAX) {
                pair.first->gameObject()->getSpatial()->scale(glm::vec3(1.1f));
            }
        }
    });
    Scene::addReceiver<MouseMessage>(nullptr, rayPickCallback);

    // Swap gravity (ctrl-g)
    auto gravSwapCallback([&](const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_G && msg.action == GLFW_PRESS && msg.mods == GLFW_MOD_CONTROL) {
            SpatialSystem::setGravity(-SpatialSystem::gravity());
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, gravSwapCallback);

    /* VSync ImGui Pane */
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "VSync",
        [&]() {
            if (ImGui::Button("VSync")) {
                Window::toggleVSync();
            }
        }
    );

    /*Parse and load json level*/
    FileReader fileReader;
    const char *levelPath = "../resources/GameLevel_02.json";
    fileReader.loadLevel(*levelPath);

    /* Create bunny */
    Mesh * bunnyMesh(Loader::getMesh("bunny.obj"));
    for (int i(0); i < 10; ++i) {
        GameObject & bunny(Scene::createGameObject());
        SpatialComponent & bunnySpatComp(Scene::addComponent<SpatialComponent>(
            bunny,
            glm::vec3(-10.0f, 5.0, i), // position
            glm::vec3(0.25f, 0.25f, 0.25f), // scale
            glm::mat3() // rotation
        ));
        NewtonianComponent & bunnyNewtComp(Scene::addComponent<NewtonianComponent>(bunny, playerMaxSpeed));
        GravityComponent & bunnyGravComp(Scene::addComponent<GravityComponent>(bunny));
        BounderComponent & bunnyBoundComp(CollisionSystem::addBounderFromMesh(bunny, 1, *bunnyMesh, false, true, false));
        DiffuseRenderComponent & bunnyDiffuse = Scene::addComponent<DiffuseRenderComponent>(
            bunny,
            RenderSystem::getShader<DiffuseShader>()->pid,
            *bunnyMesh,
            ModelTexture(0.3f, glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f)));
        /* Bunny ImGui panes */
        /*ImGuiComponent & bIc = Scene::addComponent<ImGuiComponent>(
            "Bunny", 
            [&]() {
                // Material properties
                ImGui::SliderFloat("Ambient", &bunnyDiffuse.modelTexture.material.ambient, 0.f, 1.f);
                ImGui::SliderFloat("Red", &bunnyDiffuse.modelTexture.material.diffuse.r, 0.f, 1.f);
                ImGui::SliderFloat("Green", &bunnyDiffuse.modelTexture.material.diffuse.g, 0.f, 1.f);
                ImGui::SliderFloat("Blue", &bunnyDiffuse.modelTexture.material.diffuse.b, 0.f, 1.f);
                // Spatial properties
                // dont want to be setting spat props unnecessarily
                glm::vec3 scale = bunny.getSpatial()->scale();
                if (ImGui::SliderFloat3("Scale", glm::value_ptr(scale), 1.f, 10.f)) {
                    bunny.getSpatial()->setScale(scale);
                }
                glm::vec3 position = bunny.getSpatial()->position();

                if (ImGui::SliderFloat3("Position", glm::value_ptr(position), 0.f, 10.f)) {
                    bunny.getSpatial()->setPosition(position);
                }
                static glm::vec3 axis; static float angle(0.0f);
                if (
                    ImGui::SliderFloat3("Rotation Axis", glm::value_ptr(axis), 0.0f, 1.0f) ||
                    ImGui::SliderFloat("Rotation Angle", &angle, -glm::pi<float>(), glm::pi<float>()))
                {
                    if (angle != 0.0f && axis != glm::vec3()) {
                        bunny.getSpatial()->setOrientation(glm::rotate(angle, glm::normalize(axis)));
                    }
                    else {
                        bunny.getSpatial()->setOrientation(glm::mat3());
                    }
                }
            }
        );*/
        //bunny.addComponent(bIc);
        PathfindingComponent & bunnyPathComp(Scene::addComponent<PathfindingComponent>(bunny, player, 1.0f));
    }

    /* Game stats pane */
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Stats",
        [&]() {
            ImGui::Text("FPS: %d", EngineApp::fps);
            ImGui::Text("dt: %f", EngineApp::timeStep);
        }
    );

    /* Main loop */
    EngineApp::run();

    return EXIT_SUCCESS;
}


