#include "Game.hpp"

#include "Lighting.hpp"
#include "Player.hpp"
#include "Enemies.hpp"
#include "Weapons.hpp"



namespace freecam {

    GameObject * gameObject;
    SpatialComponent * spatialComp;
    CameraComponent * cameraComp;
    CameraControllerComponent * controllerComp;

    void setup() {
        gameObject = &Scene::createGameObject();
        spatialComp = &Scene::addComponent<SpatialComponent>(*gameObject);
        cameraComp = &Scene::addComponent<CameraComponent>(*gameObject, player::k_fov, player::k_near, player::k_far);
        controllerComp = &Scene::addComponent<CameraControllerComponent>(*gameObject, player::k_lookSpeed, player::k_moveSpeed, 10.0f * player::k_moveSpeed);
        controllerComp->setEnabled(false);
    }

}



const glm::vec3 k_defGravity = glm::vec3(0.0f, -10.0f, 0.0f);

void startGame() {

    //--------------------------------------------------------------------------
    // Shader Setup

    // Diffuse shader
    if (!RenderSystem::createShader<DiffuseShader>("diffuse_vert.glsl", "diffuse_frag.glsl", lighting::lightDir)) {
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

    // Set Gravity
    SpatialSystem::setGravity(k_defGravity);

    // Load Level
    Loader::loadLevel(EngineApp::RESOURCE_DIR + "GameLevel_03.json", lighting::k_defAmbience);
    // Needs to be manually adjusted to fit level size
    CollisionSystem::setOctree(glm::vec3(-70.0f, -10.0f, -210.0f), glm::vec3(70.0f, 50.0f, 40.0f), 1.0f);
    //GameObject & go(Scene::createGameObject());
    //Scene::addComponent<SpatialComponent>(go);
    //Scene::addComponentAs<SphereBounderComponent, BounderComponent>(go, 0, Sphere(glm::vec3(0.0f, 6.0f, 0.0f), 5.0f));


    // Setup Player
    player::setup(glm::vec3(0.0f, 6.0f, 0.0f));

    // Setup Free Cam
    freecam::setup();

    // Set primary camera
    RenderSystem::setCamera(player::camera);

    // Set Sound camera
    SoundSystem::setCamera(player::camera);

    // Add Enemies
    int nEnemies(0);
    for (int i(0); i < nEnemies; ++i) {
        enemies::basic::create(glm::vec3(-(nEnemies - 1) * 0.5f + i, 5.0f, -10.0f));
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
            ImGui::Text("         Sound: %5.2f%%, %5.2f%%", Scene::        soundDT * factor, Scene::        soundMessagingDT * factor);
            ImGui::Text("    Kill Queue: %5.2f%%", Scene::killDT * factor);
            ImGui::NewLine();
            ImGui::Text("Player Pos");
            ImGui::Text("%f %f %f",
                player::spatial->position().x,
                player::spatial->position().y,
                player::spatial->position().z
            );
            ImGui::Text("Freecam Pos");
            ImGui::Text("%f %f %f",
                freecam::spatialComp->position().x,
                freecam::spatialComp->position().y,
                freecam::spatialComp->position().z
            );
        }
    );

    // Misc
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Misc",
        [&]() {
            /* Light dir */
            ImGui::SliderFloat3("LightDir", glm::value_ptr(lighting::lightDir), -1.f, 1.f);
            /* VSync */
            if (ImGui::Button("VSync")) {
                Window::toggleVSync();
            }
            /* Path finding */
            if (ImGui::Button("Turn off Path finding")) {
                enemies::disablePathfinding();
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

#endif

    //--------------------------------------------------------------------------
    // Message Handling

    // Fire projectile (click)
    // Remove all projectiles (right click)
    auto fireCallback([&](const Message & msg_) {
        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && !(msg.mods & GLFW_MOD_CONTROL) && msg.action == GLFW_PRESS) {
            weapons::grenade::firePlayer();
        }
        else if (msg.button == GLFW_MOUSE_BUTTON_2 && msg.action == GLFW_PRESS) {
            weapons::destroyAllProjectiles();
        }
    });
    Scene::addReceiver<MouseMessage>(nullptr, fireCallback);

    // Shoot ray (ctrl-click)
    int rayDepth(100);
    Vector<glm::vec3> rayPositions;
    auto rayPickCallback([&](const Message & msg_) {
        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && msg.mods & GLFW_MOD_CONTROL && msg.action == GLFW_PRESS) {
            rayPositions.clear();
            rayPositions.push_back(player::spatial->position());
            glm::vec3 dir(player::camera->getLookDir());
            for (int i(0); i < rayDepth; ++i) {
                float r(glm::distance(rayPositions.back(), glm::vec3(0.0f, 6.0f, 0.0f)));
                auto pair(CollisionSystem::pick(Ray(rayPositions.back() + dir * 0.001f, dir), player::gameObject));
                if (!pair.second.is) {
                    break;
                }
                rayPositions.push_back(pair.second.pos);
                dir = glm::normalize(glm::reflect(dir, pair.second.face ? pair.second.norm : -pair.second.norm));
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
                player::controller->setEnabled(true);
                RenderSystem::setCamera(player::camera);
            }
            else {
                // disable player controller
                player::controller->setEnabled(false);
                // enable camera object
                freecam::controllerComp->setEnabled(true);
                // set camera object camera to player camera
                freecam::spatialComp->setPosition(player::spatial->position());
                freecam::spatialComp->setUVW(player::spatial->u(), player::spatial->v(), player::spatial->w());
                freecam::cameraComp->lookInDir(player::camera->getLookDir());
                RenderSystem::setCamera(freecam::cameraComp);
            }
            free = !free;
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, camSwitchCallback);

    // Toggle gravity (ctrl-g), flip gravity (alt-g)
    auto gravCallback([&](const Message & msg_) {
        static glm::vec3 s_gravity = k_defGravity;

        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_G && msg.action == GLFW_PRESS && msg.mods == GLFW_MOD_CONTROL) {
            if (Util::isZero(SpatialSystem::gravity())) {
                SpatialSystem::setGravity(s_gravity);
            }
            else {
                s_gravity = SpatialSystem::gravity();
                SpatialSystem::setGravity(glm::vec3());
            }
        }
        else if (msg.key == GLFW_KEY_G && msg.action == GLFW_PRESS && msg.mods == GLFW_MOD_ALT) {
            s_gravity *= -1.0f;
            SpatialSystem::setGravity(-SpatialSystem::gravity());
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, gravCallback);

    // Destroy game object looking at (delete)
    auto deleteCallback([&] (const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_DELETE && msg.action == GLFW_PRESS) {
            auto pair(CollisionSystem::pick(Ray(player::spatial->position(), player::camera->getLookDir()), player::gameObject));
            if (pair.first) Scene::destroyGameObject(const_cast<GameObject &>(pair.first->gameObject()));
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, deleteCallback);

    //--------------------------------------------------------------------------

    // Main loop
    EngineApp::run();
}