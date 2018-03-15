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



}



int main(int argc, char **argv) {
    if (parseArgs(argc, argv) || EngineApp::init()) {
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }

    //--------------------------------------------------------------------------
    // Shader Setup

    // Diffuse shader
    // TODO: temporary until jaafer's pr is merged
    glm::vec3 lightDir(glm::normalize(glm::vec3(1.0f)));
    if (!RenderSystem::createShader<DiffuseShader>("diffuse_vert.glsl", "diffuse_frag.glsl", lightDir)) {
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }

    // Bounder shader
    if (!RenderSystem::createShader<BounderShader>("bounder_vert.glsl", "bounder_frag.glsl")) {
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }

    // Octree shader
    if (!RenderSystem::createShader<OctreeShader>("bounder_vert.glsl", "bounder_frag.glsl")) {
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }
    
    // Ray shader
    if (!RenderSystem::createShader<RayShader>("ray_vert.glsl", "ray_frag.glsl")) {
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }

    //--------------------------------------------------------------------------
    // General setup

    // Disable cursor
    Window::setCursorEnabled(false);

    // Load Level
    // TODO: temporary variable until this is figured out
    float ambience(0.3f);
    Loader::loadLevel(EngineApp::RESOURCE_DIR + "GameLevel_03.json", ambience);
    // Set octree. Needs to be manually adjusted to fit level size
    CollisionSystem::setOctree(glm::vec3(-70.0f, -10.0f, -210.0f), glm::vec3(70.0f, 50.0f, 40.0f), 1.0f);

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
            ImGui::Text("          Game: %5.2f%%, %5.2f%%", Scene::         gameDT * factor, Scene::         gameMessagingDT * factor);
            ImGui::Text("   Pathfinding: %5.2f%%, %5.2f%%", Scene::  pathfindingDT * factor, Scene::  pathfindingMessagingDT * factor);
            ImGui::Text("       Spatial: %5.2f%%, %5.2f%%", Scene::      spatialDT * factor, Scene::      spatialMessagingDT * factor);
            ImGui::Text("     Collision: %5.2f%%, %5.2f%%", Scene::    collisionDT * factor, Scene::    collisionMessagingDT * factor);
            ImGui::Text("Post Collision: %5.2f%%, %5.2f%%", Scene::postCollisionDT * factor, Scene::postCollisionMessagingDT * factor);
            ImGui::Text("        Render: %5.2f%%, %5.2f%%", Scene::       renderDT * factor, Scene::       renderMessagingDT * factor);
            ImGui::Text("         Sound: %5.2f%%, %5.2f%%", Scene::        soundDT * factor, Scene::        soundMessagingDT * factor);
            ImGui::Text("    Kill Queue: %5.2f%%", Scene::killDT * factor);
            ImGui::NewLine();
            ImGui::Text("Player Pos");
            // TODO
            /*ImGui::Text("%f %f %f",
                player::spatial->position().x,
                player::spatial->position().y,
                player::spatial->position().z
            );
            ImGui::Text("Freecam Pos");
            ImGui::Text("%f %f %f",
                freecam::spatialComp->position().x,
                freecam::spatialComp->position().y,
                freecam::spatialComp->position().z
            );*/
            ImGui::Text("# Picks: %d", CollisionSystem::s_nPicks);
        }
    );

    // Misc
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Misc",
        [&]() {
            /* Light dir */
            // TODO: temporarily disabled until jaafer's pr
            //ImGui::SliderFloat3("LightDir", glm::value_ptr(lighting::lightDir), -1.f, 1.f);
            /* VSync */
            if (ImGui::Button("VSync")) {
                Window::toggleVSync();
            }
            /* Path finding */
            if (ImGui::Button("Turn off Path finding")) {
                // TODO
                //enemies::disablePathfinding();
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

    // Octree shader toggle
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Octree Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                RenderSystem::getShader<OctreeShader>()->toggleEnabled();
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

    // Player health
    // TODO
    /*Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Player HP",
        [&] () {
            ImGui::ProgressBar(player::health->percentage());
        }
    );*/

#endif

    startGame();

    return EXIT_SUCCESS;
}
