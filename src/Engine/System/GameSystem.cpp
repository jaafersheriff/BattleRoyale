#include "GameSystem.hpp"

#include "Scene/Scene.hpp"



//==============================================================================
// LIGHTING

const float GameSystem::Lighting::k_defAmbience = 0.3f;
const glm::vec3 GameSystem::Lighting::k_defLightDir = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));



//==============================================================================
// PLAYER

const glm::vec3 GameSystem::Player::k_defPosition = glm::vec3(0.0f, 6.0f, 0.0f);
const float GameSystem::Player::k_defHeight = 1.75f;
const float GameSystem::Player::k_defWidth = k_defHeight / 4.0f;
const unsigned int GameSystem::Player::k_defWeight = 5;
const float GameSystem::Player::k_defLookSpeed = 0.005f;
const float GameSystem::Player::k_defMoveSpeed = 5.0f;
const float GameSystem::Player::k_defSprintSpeed = 15.0f;
const float GameSystem::Player::k_defJumpSpeed = 5.0f;
const float GameSystem::Player::k_defFOV = 45.0f;
const float GameSystem::Player::k_defNear = 0.1f;
const float GameSystem::Player::k_defFar = 300.0f;
const float GameSystem::Player::k_defMaxHP = 100.0f;
const glm::vec3 GameSystem::Player::k_defPlayerPosition = glm::vec3(0.0f, 6.0f, 0.0f);
const glm::vec3 GameSystem::Player::k_defMainHandPosition = glm::vec3(k_defWidth * 0.5f, 0.0f, -k_defWidth * 0.5f);

GameObject * GameSystem::Player::gameObject = nullptr;
SpatialComponent * GameSystem::Player::spatial = nullptr;
NewtonianComponent * GameSystem::Player::newtonian = nullptr;
CapsuleBounderComponent * GameSystem::Player::bounder = nullptr;
CameraComponent * GameSystem::Player::camera = nullptr;
PlayerControllerComponent * GameSystem::Player::controller = nullptr;
PlayerComponent * GameSystem::Player::playerComp = nullptr;
HealthComponent * GameSystem::Player::health = nullptr;

void GameSystem::Player::init() {
    gameObject = &Scene::createGameObject();
    spatial = &Scene::addComponent<SpatialComponent>(*gameObject, k_defPosition);
    newtonian = &Scene::addComponent<NewtonianComponent>(*gameObject, false);
    Scene::addComponentAs<GravityComponent, AcceleratorComponent>(*gameObject);
    Scene::addComponent<GroundComponent>(*gameObject);
    Capsule playerCap(glm::vec3(0.0f, -k_defHeight * 0.5f + k_defWidth, 0.0f), k_defWidth, k_defHeight - 2.0f * k_defWidth);
    bounder = &Scene::addComponentAs<CapsuleBounderComponent, BounderComponent>(*gameObject, k_defWeight, playerCap);
    camera = &Scene::addComponent<CameraComponent>(*gameObject, k_defFOV, k_defNear, k_defFar);
    controller = &Scene::addComponent<PlayerControllerComponent>(*gameObject, k_defLookSpeed, k_defMoveSpeed, k_defJumpSpeed, k_defSprintSpeed);
    playerComp = &Scene::addComponent<PlayerComponent>(*gameObject);
    health = &Scene::addComponent<HealthComponent>(*gameObject, k_defMaxHP);
}



//==============================================================================
// ENEMIES

//------------------------------------------------------------------------------
// Basic

const String GameSystem::Enemies::Basic::k_defMeshName = "bunny.obj";
const bool GameSystem::Enemies::Basic::k_defIsToon = true;
const glm::vec3 GameSystem::Enemies::Basic::k_defScale = glm::vec3(0.75f);
const unsigned int GameSystem::Enemies::Basic::k_defWeight = 5;
const float GameSystem::Enemies::Basic::k_defMoveSpeed = 3.0f;
const float GameSystem::Enemies::Basic::k_defMaxHP = 100.0f;

void GameSystem::Enemies::Basic::create(const glm::vec3 & position) {
    const Mesh * mesh(Loader::getMesh(k_defMeshName));
    const DiffuseShader * shader();
    ModelTexture modelTex(Lighting::k_defAmbience, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f));
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, position, k_defScale));
    NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj, false));
    GravityComponent & gravComp(Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj));
    BounderComponent & boundComp(CollisionSystem::addBounderFromMesh(obj, k_defWeight, *mesh, false, true, false));
    PathfindingComponent & pathComp(Scene::addComponent<PathfindingComponent>(obj, *Player::gameObject, k_defMoveSpeed, false));
    DiffuseRenderComponent & renderComp = Scene::addComponent<DiffuseRenderComponent>(
        obj,
        spatComp,
        RenderSystem::getShader<DiffuseShader>()->pid,
        *mesh,
        modelTex,
        k_defIsToon,
        glm::vec2(1.0f)
    );
    HealthComponent & healthComp(Scene::addComponent<HealthComponent>(obj, k_defMaxHP));
    EnemyComponent & enemyComp(Scene::addComponentAs<BasicEnemyComponent, EnemyComponent>(obj));
}

void GameSystem::Enemies::Basic::spawn() {
    glm::vec3 dir(-Player::spatial->w());
    dir = Util::safeNorm(glm::vec3(dir.x, 0.0f, dir.z));
    if (dir == glm::vec3()) {
        return;
    }
    auto pair(CollisionSystem::pickHeavy(Ray(Player::spatial->position(), dir), UINT_MAX));
    Intersect & inter(pair.second);
    if (inter.dist > 20.0f) {
        create(Player::spatial->position() + dir * 20.0f);
    }    
}

//------------------------------------------------------------------------------
// All

void GameSystem::Enemies::enablePathfinding() {
    for (EnemyComponent * comp : s_enemyComponents) {
        GameObject & enemy(comp->gameObject());
        PathfindingComponent * path(enemy.getComponentByType<PathfindingComponent>());
        if (!path) {
            Scene::addComponent<PathfindingComponent>(enemy, *Player::gameObject, Basic::k_defMoveSpeed, false);
        }
    }
}

void GameSystem::Enemies::disablePathfinding() {
    for (EnemyComponent * comp : s_enemyComponents) {
        GameObject & enemy(comp->gameObject());
        PathfindingComponent * path(enemy.getComponentByType<PathfindingComponent>());
        if (path) {
            Scene::removeComponent(*path);
        }
    }
}



//==============================================================================
// WEAPONS

//------------------------------------------------------------------------------
// Grenade

const String GameSystem::Weapons::Grenade::k_defMeshName = "weapons/SodaCan.obj";
const String GameSystem::Weapons::Grenade::k_defTexName = "weapons/SodaCan_Tex.png";
const bool GameSystem::Weapons::Grenade::k_defIsToon = true;
const glm::vec3 GameSystem::Weapons::Grenade::k_defScale = glm::vec3(1.0f);
const unsigned int GameSystem::Weapons::Grenade::k_defWeight = 1;
const float GameSystem::Weapons::Grenade::k_defSpeed = 20.0f; 
const float GameSystem::Weapons::Grenade::k_defDamage = 50.0f;
const float GameSystem::Weapons::Grenade::k_defRadius = 5.0f;

void GameSystem::Weapons::Grenade::fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel) {
    const Mesh * mesh(Loader::getMesh(k_defMeshName));
    const Texture * tex(Loader::getTexture(k_defTexName));
    ModelTexture modelTex(tex, Lighting::k_defAmbience, glm::vec3(1.0f), glm::vec3(1.0f));
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, initPos, k_defScale));
    BounderComponent & bounderComp(CollisionSystem::addBounderFromMesh(obj, k_defWeight, *mesh, false, true, false));
    NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj, true));
    GroundComponent & groundComp(Scene::addComponent<GroundComponent>(obj));
    Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj);
    newtComp.addVelocity(initDir * k_defSpeed + srcVel);
    DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj,
        spatComp,
        RenderSystem::getShader<DiffuseShader>()->pid,
        *mesh,
        modelTex,
        k_defIsToon,
        glm::vec2(1.0f)
    ));
    ProjectileComponent & weaponComp(Scene::addComponentAs<GrenadeComponent, ProjectileComponent>(obj, k_defDamage, k_defRadius));
}

void GameSystem::Weapons::Grenade::fireFromPlayer() {
    glm::vec3 initPos(Player::camera->orientMatrix() * Player::k_defMainHandPosition + Player::spatial->position());
    fire(initPos, Player::camera->getLookDir(), Player::spatial->effectiveVelocity());
}

//------------------------------------------------------------------------------
// All

void GameSystem::Weapons::destroyAllProjectiles() {        
    for (ProjectileComponent * comp : s_projectileComponents) {
        Scene::destroyGameObject(comp->gameObject());
    }
    for (BlastComponent * comp : s_blastComponents) {
        Scene::destroyGameObject(comp->gameObject());
    }
}



//==============================================================================
// FREECAM

const float GameSystem::Freecam::k_defMinSpeed = 0.25f;
const float GameSystem::Freecam::k_defMaxSpeed = 100.0f;

GameObject * GameSystem::Freecam::gameObject = nullptr;
SpatialComponent * GameSystem::Freecam::spatialComp = nullptr;
CameraComponent * GameSystem::Freecam::cameraComp = nullptr;
CameraControllerComponent * GameSystem::Freecam::controllerComp = nullptr;

void GameSystem::Freecam::init() {
    gameObject = &Scene::createGameObject();
    spatialComp = &Scene::addComponent<SpatialComponent>(*gameObject);
    cameraComp = &Scene::addComponent<CameraComponent>(*gameObject, Player::k_defFOV, Player::k_defNear, Player::k_defFar);
    controllerComp = &Scene::addComponent<CameraControllerComponent>(*gameObject, Player::k_defLookSpeed, k_defMinSpeed, k_defMaxSpeed);
    controllerComp->setEnabled(false);
}



//==============================================================================
// Game System
 
const glm::vec3 GameSystem::k_defGravity = glm::vec3(0.0f, -10.0f, 0.0f);

const Vector<CameraComponent *> & GameSystem::s_cameraComponents(Scene::getComponents<CameraComponent>());
const Vector<CameraControllerComponent *> & GameSystem::s_cameraControllerComponents(Scene::getComponents<CameraControllerComponent>());
const Vector<PlayerControllerComponent *> & GameSystem::s_playerControllers(Scene::getComponents<PlayerControllerComponent>());
const Vector<PlayerComponent *> & GameSystem::s_playerComponents(Scene::getComponents<PlayerComponent>());
const Vector<EnemyComponent *> & GameSystem::s_enemyComponents(Scene::getComponents<EnemyComponent>());
const Vector<ProjectileComponent *> & GameSystem::s_projectileComponents(Scene::getComponents<ProjectileComponent>());
const Vector<BlastComponent *> & GameSystem::s_blastComponents(Scene::getComponents<BlastComponent>());

void GameSystem::init() {

    //--------------------------------------------------------------------------
    // General setup

    // Disable cursor
    Window::setCursorEnabled(false);

    // Set gravity
    SpatialSystem::setGravity(k_defGravity);

    // Init player
    Player::init();

    // Init Free Cam
    Freecam::init();

    // Set primary camera
    RenderSystem::setCamera(Player::camera);

    // Set Sound camera
    SoundSystem::setCamera(Player::camera);

    // Load Level
    Loader::loadLevel(EngineApp::RESOURCE_DIR + "GameLevel_03.json", Lighting::k_defAmbience);
    // Set octree. Needs to be manually adjusted to fit level size
    CollisionSystem::setOctree(glm::vec3(-70.0f, -10.0f, -210.0f), glm::vec3(70.0f, 50.0f, 40.0f), 1.0f);

    // Set message callbacks
    Scene::addReceiver<MouseMessage>(nullptr, fireCallback);
    Scene::addReceiver<KeyMessage>(nullptr, spawnEnemy);
    Scene::addReceiver<MouseMessage>(nullptr, rayPickCallback);
    Scene::addReceiver<KeyMessage>(nullptr, camSwitchCallback);
    Scene::addReceiver<KeyMessage>(nullptr, gravCallback);

    setupImGui();
}

void GameSystem::update(float dt) {
    for (auto & comp : s_playerControllers) {
        comp->update(dt);
    }
    for (auto & comp : s_cameraControllerComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_cameraComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_playerComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_enemyComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_projectileComponents) {
        comp->update(dt);
    }
    for (auto & comp : s_blastComponents) {
        comp->update(dt);
    }
}



//==============================================================================
// Message Handling

// Fire projectile (click)
// Remove all projectiles (right click)
void GameSystem::fireCallback(const Message & msg_) {
    const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
    if (msg.button == GLFW_MOUSE_BUTTON_1 && !(msg.mods & GLFW_MOD_CONTROL) && msg.action == GLFW_PRESS) {
        Weapons::Grenade::fireFromPlayer();
    }
    else if (msg.button == GLFW_MOUSE_BUTTON_2 && msg.action == GLFW_PRESS) {
        Weapons::destroyAllProjectiles();
    }
};

// Spawn enemy (1)
void GameSystem::spawnEnemy(const Message & msg_) {
    const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
    if (msg.key == GLFW_KEY_1 && msg.action == GLFW_PRESS) {
        Enemies::Basic::spawn();
    }
};

// Shoot ray (ctrl-click)
int rayDepth(100);
Vector<glm::vec3> rayPositions;
void GameSystem::rayPickCallback(const Message & msg_) {
    const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
    if (msg.button == GLFW_MOUSE_BUTTON_1 && msg.mods & GLFW_MOD_CONTROL && msg.action == GLFW_PRESS) {
        rayPositions.clear();
        rayPositions.push_back(Player::spatial->position());
        glm::vec3 dir(Player::camera->getLookDir());
        for (int i(0); i < rayDepth; ++i) {
            auto pair(CollisionSystem::pick(Ray(rayPositions.back() + dir * 0.001f, dir)));
            if (!pair.second.is) {
                break;
            }
            rayPositions.push_back(pair.second.pos);
            dir = glm::normalize(glm::reflect(dir, pair.second.face ? pair.second.norm : -pair.second.norm));
        }
        RenderSystem::getShader<RayShader>()->setPositions(rayPositions);
    }
};

// Toggle Freecam (ctrl-tab)
void GameSystem::camSwitchCallback(const Message & msg_) {
    static bool free = false;

    const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
    if (msg.key == GLFW_KEY_TAB && msg.action == GLFW_PRESS && msg.mods & GLFW_MOD_CONTROL) {
        if (free) {
            // disable camera controller
            Freecam::controllerComp->setEnabled(false);
            // enable player controller
            Player::controller->setEnabled(true);
            RenderSystem::setCamera(Player::camera);
        }
        else {
            // disable player controller
            Player::controller->setEnabled(false);
            // enable camera object
            Freecam::controllerComp->setEnabled(true);
            // set camera object camera to player camera
            Freecam::spatialComp->setPosition(Player::spatial->position());
            Freecam::spatialComp->setUVW(Player::spatial->u(), Player::spatial->v(), Player::spatial->w());
            Freecam::cameraComp->lookInDir(Player::camera->getLookDir());
            RenderSystem::setCamera(Freecam::cameraComp);
        }
        free = !free;
    }
};

// Toggle gravity (ctrl-g), flip gravity (alt-g)
void GameSystem::gravCallback(const Message & msg_) {
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
};



//==============================================================================
// ImGui

void GameSystem::setupImGui() {
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
            ImGui::Text("%f %f %f",
                Player::spatial->position().x,
                Player::spatial->position().y,
                Player::spatial->position().z
            );
            ImGui::Text("Freecam Pos");
            ImGui::Text("%f %f %f",
                Freecam::spatialComp->position().x,
                Freecam::spatialComp->position().y,
                Freecam::spatialComp->position().z
            );
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
                Enemies::disablePathfinding();
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
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Player HP",
        [&] () {
            ImGui::ProgressBar(Player::health->percentage());
        }
    );

#endif
}