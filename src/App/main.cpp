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
            EngineApp::get().verbose = true;
        }
        /* Set resource dir */
        if (!strcmp(argv[i], "-r")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            EngineApp::get().RESOURCE_DIR = argv[i + 1];
        }
        /* Set application name */
        if (!strcmp(argv[i], "-n")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            EngineApp::get().APP_NAME = argv[i + 1];
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    /* Singular engine */
    EngineApp & engine(EngineApp::get());

    if (parseArgs(argc, argv) || engine.init()) {
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }

    Window::setCursorEnabled(false);

    /* Scene reference for QOL */
    Scene & scene(Scene::get());

    /* Create diffuse shader */
    glm::vec3 lightPos(100.f, 100.f, 100.f);
    // TODO : user shouldn't need to specify resource dir here
    if (!RenderSystem::get().createShader<DiffuseShader>(
            engine.RESOURCE_DIR + "diffuse_vert.glsl",    /* Vertex shader file       */
            engine.RESOURCE_DIR + "diffuse_frag.glsl",    /* Fragment shader file     */
            lightPos                                      /* Shader-specific uniforms */
        )) {
        std::cerr << "Failed to add diffuse shader" << std::endl;
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }
    /* Diffuse Shader ImGui Pane */
    scene.createComponent<ImGuiComponent>(
        "Diffuse Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                RenderSystem::get().getShader<DiffuseShader>()->toggleEnabled();
            }
            if (ImGui::Button("Wireframe")) {
                RenderSystem::get().getShader<DiffuseShader>()->toggleWireFrame();
            }
       }
    );

    // Create collider
    // alternate method using unique_ptr and new
    if (!RenderSystem::get().addShader(std::unique_ptr<BounderShader>(new BounderShader(
            engine.RESOURCE_DIR + "bounder_vert.glsl",
            engine.RESOURCE_DIR + "bounder_frag.glsl"
        )))) {
        std::cerr << "Failed to add collider shader" << std::endl;
        std::cin.get(); //don't immediately close the console
        return EXIT_FAILURE;
    }
    /* Collider ImGui pane */
    scene.createComponent<ImGuiComponent>(
        "Bounder Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                RenderSystem::get().getShader<BounderShader>()->toggleEnabled();
            }
        }
    );

    /* Setup Camera */
    float camFOV(45.0f);
    float camLookSpeed(0.2f);
    float camMoveSpeed(15.0f);
    GameObject & camera(scene.createGameObject());
    SpatialComponent & camSpatComp(scene.createComponent<SpatialComponent>());
    camera.addComponent(camSpatComp);
    CameraComponent & camCamComp(scene.createComponent<CameraComponent>(camSpatComp, camFOV));
    camera.addComponent(camCamComp);
    CameraControllerComponent & camContComp(scene.createComponent<CameraControllerComponent>(camCamComp, camLookSpeed, camMoveSpeed));
    camera.addComponent(camContComp);
    camContComp.setEnabled(false);

    /* Setup Player */
    float playerHeight(1.75f);
    float playerWidth(playerHeight / 4.0f);
    float playerFOV(camFOV);
    float playerLookSpeed(camLookSpeed);
    float playerMoveSpeed(camMoveSpeed);
    GameObject & player(scene.createGameObject());
    SpatialComponent & playerSpatComp(scene.createComponent<SpatialComponent>());
    player.addComponent(playerSpatComp);
    playerSpatComp.setPosition(glm::vec3(-4.0f, 6.0f, 0.0f));
    Capsule playerCap(glm::vec3(), playerHeight - 2.0f * playerWidth, playerWidth);
    CapsuleBounderComponent & playerBoundComp(scene.createComponent<CapsuleBounderComponent>(*player.getSpatial(), 1, playerCap));
    player.addComponent(playerBoundComp);
    CameraComponent & playerCamComp(scene.createComponent<CameraComponent>(playerSpatComp, playerFOV));
    player.addComponent(playerCamComp);
    PlayerControllerComponent & playerContComp(scene.createComponent<PlayerControllerComponent>(playerCamComp, playerLookSpeed, playerMoveSpeed));
    player.addComponent(playerContComp);

    RenderSystem::get().setCamera(&playerCamComp);

    // press shift-tab to toggle free camera
    auto camSwitchCallback([&](const Message & msg_) {
        static bool free = false;

        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_TAB && msg.action == GLFW_PRESS && msg.mods & GLFW_MOD_CONTROL) {
            if (free) {
                // disable camera controller
                camContComp.setEnabled(false);
                // enable player controller
                playerContComp.setEnabled(true);
                RenderSystem::get().setCamera(&playerCamComp);
            }
            else {
                // disable player controller
                playerContComp.setEnabled(false);
                // enable camera object
                camContComp.setEnabled(true);
                // set camera object camera to player camera
                camSpatComp.setPosition(playerSpatComp.position());
                camSpatComp.setUVW(playerSpatComp.u(), playerSpatComp.v(), playerSpatComp.w());
                camCamComp.lookInDir(playerCamComp.getLookDir());
                RenderSystem::get().setCamera(&camCamComp);
            }
            free = !free;
        }
    });
    scene.addReceiver<KeyMessage>(nullptr, camSwitchCallback);

    /* VSync ImGui Pane */
    scene.createComponent<ImGuiComponent>(
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
    fileReader.loadLevel(*levelPath, scene);

    /* Create bunny */
    Mesh * bunnyMesh(Loader::getMesh("bunny.obj"));
    GameObject & bunny(scene.createGameObject());
    bunny.addComponent(scene.createComponent<SpatialComponent>(
        glm::vec3(0.0f, 0.0f, 0.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f), // scale
        glm::mat3() // rotation
    ));
    bunny.addComponent(Scene::get().addComponent<BounderComponent>(createBounderFromMesh(*bunny.getSpatial(), 1, *bunnyMesh, false, true, false)));
    DiffuseRenderComponent & bunnyDiffuse = scene.createComponent<DiffuseRenderComponent>(
        RenderSystem::get().getShader<DiffuseShader>()->pid,
        *bunnyMesh,
        ModelTexture(0.3f, glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f)));
    bunny.addComponent(bunnyDiffuse);
    /* Bunny ImGui panes */
    ImGuiComponent & bIc = scene.createComponent<ImGuiComponent>(
        "Bunny", 
        [&]() {
            /* Material properties */
            ImGui::SliderFloat("Ambient", &bunnyDiffuse.modelTexture.material.ambient, 0.f, 1.f);
            ImGui::SliderFloat("Red", &bunnyDiffuse.modelTexture.material.diffuse.r, 0.f, 1.f);
            ImGui::SliderFloat("Green", &bunnyDiffuse.modelTexture.material.diffuse.g, 0.f, 1.f);
            ImGui::SliderFloat("Blue", &bunnyDiffuse.modelTexture.material.diffuse.b, 0.f, 1.f);
            /* Spatial properties */
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
    );
    bunny.addComponent(bIc);
    bunny.addComponent(scene.createComponent<PathfindingComponent>(player, 1.0f));

    /* Game stats pane */
    scene.createComponent<ImGuiComponent>(
        "Stats",
        [&]() {
            ImGui::Text("FPS: %d", engine.fps);
            ImGui::Text("dt: %f", engine.timeStep);
        }
    );

    /* Main loop */
    engine.run();

    return EXIT_SUCCESS;
}


