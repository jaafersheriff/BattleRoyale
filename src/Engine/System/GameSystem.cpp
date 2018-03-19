#include "GameSystem.hpp"

#include "glm/gtc/type_ptr.hpp"

#include "Scene/Scene.hpp"
#include "Systems.hpp"
#include "Shaders/Shaders.hpp"
#include "Component/Components.hpp"
#include "Loader/Loader.hpp"
#include "Util/Util.hpp"
#include "IO/Window.hpp"
#include "EngineApp/EngineApp.hpp"



//==============================================================================
// LIGHTING

const float GameSystem::Lighting::k_defAmbience = 0.3f;
const Material GameSystem::Lighting::k_defMaterial = Material(glm::vec3(1.0f), glm::vec3(1.0f), 16.0f);
const glm::vec3 GameSystem::Lighting::k_defLightDir = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));



//==============================================================================
// PLAYER

const glm::vec3 GameSystem::Player::k_position = glm::vec3(0.0f, 6.0f, 0.0f);
const float GameSystem::Player::k_height = 1.75f;
const float GameSystem::Player::k_width = k_height / 4.0f;
const unsigned int GameSystem::Player::k_weight = 5;
const float GameSystem::Player::k_lookSpeed = 0.005f;
const float GameSystem::Player::k_moveSpeed = 5.0f;
const float GameSystem::Player::k_sprintSpeed = 15.0f;
const float GameSystem::Player::k_jumpSpeed = 5.0f;
const float GameSystem::Player::k_fov = 45.0f;
const float GameSystem::Player::k_near = 0.1f;
const float GameSystem::Player::k_far = 300.0f;
const float GameSystem::Player::k_maxHP = 100.0f;
const glm::vec3 GameSystem::Player::k_playerPosition = glm::vec3(0.0f, 6.0f, 0.0f);
const glm::vec3 GameSystem::Player::k_mainHandPosition = glm::vec3(k_width * 0.5f, -k_height * 0.1f, -k_width * 0.5f);

GameObject * GameSystem::Player::gameObject = nullptr;
SpatialComponent * GameSystem::Player::spatial = nullptr;
NewtonianComponent * GameSystem::Player::newtonian = nullptr;
CapsuleBounderComponent * GameSystem::Player::bounder = nullptr;
CameraComponent * GameSystem::Player::camera = nullptr;
PlayerControllerComponent * GameSystem::Player::controller = nullptr;
PlayerComponent * GameSystem::Player::playerComp = nullptr;
HealthComponent * GameSystem::Player::health = nullptr;
SpatialComponent * GameSystem::Player::handSpatial = nullptr;

void GameSystem::Player::init() {
    gameObject = &Scene::createGameObject();
    spatial = &Scene::addComponent<SpatialComponent>(*gameObject, k_position);
    newtonian = &Scene::addComponent<NewtonianComponent>(*gameObject, false);
    Scene::addComponentAs<GravityComponent, AcceleratorComponent>(*gameObject);
    Scene::addComponent<GroundComponent>(*gameObject);
    Capsule playerCap(glm::vec3(0.0f, -k_height * 0.5f + k_width, 0.0f), k_width, k_height - 2.0f * k_width);
    bounder = &Scene::addComponentAs<CapsuleBounderComponent, BounderComponent>(*gameObject, k_weight, playerCap);
    camera = &Scene::addComponent<CameraComponent>(*gameObject, k_fov, k_near, k_far);
    controller = &Scene::addComponent<PlayerControllerComponent>(*gameObject, k_lookSpeed, k_moveSpeed, k_jumpSpeed, k_sprintSpeed);
    playerComp = &Scene::addComponent<PlayerComponent>(*gameObject);
    health = &Scene::addComponent<HealthComponent>(*gameObject, k_maxHP);
    handSpatial = &Scene::addComponent<SpatialComponent>(*gameObject, k_mainHandPosition, spatial);
    const Mesh * handMesh(Loader::getMesh("weapons/Pizza_Slice.obj"));
    const Texture * handTex(Loader::getTexture("weapons/Pizza_Tex.png"));
    ModelTexture handModelTex(handTex, Lighting::k_defMaterial);
    Scene::addComponent<DiffuseRenderComponent>(*gameObject,
        *handSpatial,
        *handMesh,
        handModelTex,
        true,
        glm::vec2(1.0f)
    );
}



//==============================================================================
// ENEMIES

//------------------------------------------------------------------------------
// Basic

const String GameSystem::Enemies::Basic::k_bodyMeshName = "characters/Enemy_Torso.obj";
const String GameSystem::Enemies::Basic::k_headMeshName = "characters/Enemy_Head.obj";
const String GameSystem::Enemies::Basic::k_textureName = "characters/Enemy_Tex.png";
const glm::vec3 GameSystem::Enemies::Basic::k_headPosition = glm::vec3(0.0f, 1.0f, 0.0f);
const bool GameSystem::Enemies::Basic::k_isToon = true;
const glm::vec3 GameSystem::Enemies::Basic::k_scale = glm::vec3(0.75f);
const unsigned int GameSystem::Enemies::Basic::k_weight = 5;
const float GameSystem::Enemies::Basic::k_moveSpeed = 3.0f;
const float GameSystem::Enemies::Basic::k_maxHP = 100.0f;

void GameSystem::Enemies::Basic::create(const glm::vec3 & position) {
    const Mesh * bodyMesh(Loader::getMesh(k_bodyMeshName));
    const Mesh * headMesh(Loader::getMesh(k_headMeshName));
    const Texture * texture(Loader::getTexture(k_textureName));
    const DiffuseShader * shader();
    ModelTexture modelTex(texture, Lighting::k_defMaterial);
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & bodySpatComp(Scene::addComponent<SpatialComponent>(obj, position, k_scale));
    SpatialComponent & headSpatComp(Scene::addComponent<SpatialComponent>(obj, k_headPosition, &bodySpatComp));
    NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj, false));
    GravityComponent & gravComp(Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj));
    BounderComponent & bodyBoundComp(CollisionSystem::addBounderFromMesh(obj, k_weight, *bodyMesh, false, false, true));
    BounderComponent & headBoundComp(CollisionSystem::addBounderFromMesh(obj, k_weight, *headMesh, false, true, false, &headSpatComp));
    PathfindingComponent & pathComp(Scene::addComponent<PathfindingComponent>(obj, *Player::gameObject, k_moveSpeed, false));
    DiffuseRenderComponent & bodyRenderComp = Scene::addComponent<DiffuseRenderComponent>(
        obj,
        bodySpatComp,
        *bodyMesh,
        modelTex,
        k_isToon,
        glm::vec2(1.0f)
    );
    DiffuseRenderComponent & headRenderComp = Scene::addComponent<DiffuseRenderComponent>(
        obj,
        headSpatComp,
        *headMesh,
        modelTex,
        k_isToon,
        glm::vec2(1.0f)
    );
    HealthComponent & healthComp(Scene::addComponent<HealthComponent>(obj, k_maxHP));
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
            Scene::addComponent<PathfindingComponent>(enemy, *Player::gameObject, Basic::k_moveSpeed, false);
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
// Pizza Slice

const String GameSystem::Weapons::PizzaSlice::k_meshName = "weapons/Pizza_Slice.obj";
const String GameSystem::Weapons::PizzaSlice::k_texName = "weapons/Pizza_Tex.png";
const bool GameSystem::Weapons::PizzaSlice::k_isToon = true;
const glm::vec3 GameSystem::Weapons::PizzaSlice::k_scale = glm::vec3(1.0f);
const unsigned int GameSystem::Weapons::PizzaSlice::k_weight = 0;
const float GameSystem::Weapons::PizzaSlice::k_speed = 30.0f; 
const float GameSystem::Weapons::PizzaSlice::k_damage = 25.0f;

void GameSystem::Weapons::PizzaSlice::fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel) {
    const Mesh * mesh(Loader::getMesh(k_meshName));
    const Texture * tex(Loader::getTexture(k_texName));
    ModelTexture modelTex(tex, Lighting::k_defMaterial);
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, initPos, k_scale));
    BounderComponent & bounderComp(CollisionSystem::addBounderFromMesh(obj, k_weight, *mesh, false, true, false));
    NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj, true));
    GroundComponent & groundComp(Scene::addComponent<GroundComponent>(obj));
    newtComp.addVelocity(initDir * k_speed + srcVel);
    DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj,
        spatComp,
        *mesh,
        modelTex,
        k_isToon,
        glm::vec2(1.0f)
    ));
    ProjectileComponent & weaponComp(Scene::addComponentAs<BulletComponent, ProjectileComponent>(obj, k_damage));

    SoundSystem::playSound3D("splash2.wav", spatComp.position());
}

void GameSystem::Weapons::PizzaSlice::fireFromPlayer() {
    glm::vec3 initPos(Player::camera->orientMatrix() * Player::k_mainHandPosition + Player::spatial->position());
    fire(initPos, Player::camera->getLookDir(), Player::spatial->effectiveVelocity());
}

//------------------------------------------------------------------------------
// Grenade

const String GameSystem::Weapons::SodaGrenade::k_meshName = "weapons/SodaCan.obj";
const String GameSystem::Weapons::SodaGrenade::k_texName = "weapons/SodaCan_Tex.png";
const bool GameSystem::Weapons::SodaGrenade::k_isToon = true;
const glm::vec3 GameSystem::Weapons::SodaGrenade::k_scale = glm::vec3(1.0f);
const unsigned int GameSystem::Weapons::SodaGrenade::k_weight = 1;
const float GameSystem::Weapons::SodaGrenade::k_speed = 20.0f; 
const float GameSystem::Weapons::SodaGrenade::k_damage = 50.0f;
const float GameSystem::Weapons::SodaGrenade::k_radius = 5.0f;

void GameSystem::Weapons::SodaGrenade::fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel) {
    const Mesh * mesh(Loader::getMesh(k_meshName));
    const Texture * tex(Loader::getTexture(k_texName));
    ModelTexture modelTex(tex, Lighting::k_defMaterial);
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, initPos, k_scale));
    BounderComponent & bounderComp(CollisionSystem::addBounderFromMesh(obj, k_weight, *mesh, false, true, false));
    NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj, true));
    GroundComponent & groundComp(Scene::addComponent<GroundComponent>(obj));
    Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj);
    newtComp.addVelocity(initDir * k_speed + srcVel);
    DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj,
        spatComp,
        *mesh,
        modelTex,
        k_isToon,
        glm::vec2(1.0f)
    ));
    ProjectileComponent & weaponComp(Scene::addComponentAs<GrenadeComponent, ProjectileComponent>(obj, k_damage, k_radius));
    SpinAnimationComponent & spinAnimation(Scene::addComponentAs<SpinAnimationComponent, AnimationComponent>(obj, spatComp, glm::vec3(1.0f, 0.0f, 0.0f), -5.0f));

    SoundSystem::playSound3D("sword_slash.wav", spatComp.position());
}

void GameSystem::Weapons::SodaGrenade::fireFromPlayer() {
    glm::vec3 initPos(Player::camera->orientMatrix() * Player::k_mainHandPosition + Player::spatial->position());
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

const float GameSystem::Freecam::k_minSpeed = 0.25f;
const float GameSystem::Freecam::k_maxSpeed = 100.0f;

GameObject * GameSystem::Freecam::gameObject = nullptr;
SpatialComponent * GameSystem::Freecam::spatialComp = nullptr;
CameraComponent * GameSystem::Freecam::cameraComp = nullptr;
CameraControllerComponent * GameSystem::Freecam::controllerComp = nullptr;

void GameSystem::Freecam::init() {
    gameObject = &Scene::createGameObject();
    spatialComp = &Scene::addComponent<SpatialComponent>(*gameObject);
    cameraComp = &Scene::addComponent<CameraComponent>(*gameObject, Player::k_fov, Player::k_near, Player::k_far);
    controllerComp = &Scene::addComponent<CameraControllerComponent>(*gameObject, Player::k_lookSpeed, k_minSpeed, k_maxSpeed);
    controllerComp->setEnabled(false);
}



//==============================================================================
// MUSIC

const String GameSystem::Music::k_defMusic = "bgRock1.mp3";

bool GameSystem::Music::s_playing = false;

void GameSystem::Music::start() {
    SoundSystem::setBackgroundMusic(k_defMusic, true);
    SoundSystem::playBackgroundMusic();
    SoundSystem::setBackgroundMusicVolume(0.05f);
    s_playing = true;
}

void GameSystem::Music::stop() {
    SoundSystem::pauseBackgroundMusic();
    s_playing = false;
}

void GameSystem::Music::toggle() {
    s_playing = !s_playing;
    if (s_playing) start();
    else stop();
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
    Loader::loadLevel(EngineApp::RESOURCE_DIR + "GameLevel_03.json");
    // Set octree. Needs to be manually adjusted to fit level size
    CollisionSystem::setOctree(glm::vec3(-70.0f, -10.0f, -210.0f), glm::vec3(70.0f, 50.0f, 40.0f), 1.0f);

    // Start music
    Music::start();

    // Set message callbacks
    setupMessageCallbacks();
    
    // Setup ImGui
    setupImGui();

    // Disable certain shaders
    //RenderSystem::s_bounderShader->setEnabled(false);
    RenderSystem::s_octreeShader->setEnabled(false);
    RenderSystem::s_rayShader->setEnabled(false);

    // Water fountain
    GameObject & fountain(Scene::createGameObject());
    SpatialComponent & fountainSpat(Scene::addComponent<SpatialComponent>(fountain, glm::vec3(1.0f, 7.6f, -51.0f)));
    fountainSpat.setRelativeUVW(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), true);
    ParticleSystem::addWaterFountainPC(fountainSpat);
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

void GameSystem::setupMessageCallbacks() {

    // Shoot pizza slice (click) or fire soda grenade (right-click)
    auto fireCallback([&](const Message & msg_) {
        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && !(msg.mods & GLFW_MOD_CONTROL) && msg.action == GLFW_PRESS) {
            Weapons::PizzaSlice::fireFromPlayer();
        }
        else if (msg.button == GLFW_MOUSE_BUTTON_2 && !(msg.mods & GLFW_MOD_CONTROL) && msg.action == GLFW_PRESS) {
            Weapons::SodaGrenade::fireFromPlayer();
        }
    });
    Scene::addReceiver<MouseMessage>(nullptr, fireCallback);

    // Spawn enemy (1)
    auto spawnEnemyCallback([&](const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_1 && msg.action == GLFW_PRESS) {
            Enemies::Basic::spawn();
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, spawnEnemyCallback);

    // Shoot ray (ctrl-click)
    auto rayPickCallback([&](const Message & msg_) {
        static const int rayDepth(100);
        static Vector<glm::vec3> rayPositions;

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
            RenderSystem::s_rayShader->setPositions(rayPositions);
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
                Freecam::spatialComp->setRelativePosition(Player::spatial->position());
                Freecam::spatialComp->setRelativeUVW(Player::spatial->u(), Player::spatial->v(), Player::spatial->w());
                Freecam::cameraComp->lookInDir(Player::camera->getLookDir());
                RenderSystem::setCamera(Freecam::cameraComp);
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

    // Toggle background music (m)
    auto musicCallback([&](const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));

        if (msg.key == GLFW_KEY_M && !msg.mods && msg.action == GLFW_PRESS) {
            Music::toggle();
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, musicCallback);

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
            ImGui::Text("# Picks: %d", CollisionSystem::s_nPicks);
            ImGui::NewLine();
            ImGui::Text("Game Objects: %d", Scene::getGameObjects().size());
            ImGui::Text("Components");
            ImGui::Text("     Spatial: %d", Scene::getComponents<SpatialComponent>().size());
            ImGui::Text("     Diffuse: %d", Scene::getComponents<DiffuseRenderComponent>().size());
            ImGui::Text("     Bounder: %d", Scene::getComponents<BounderComponent>().size());
            ImGui::Text("    Particle: %d", Scene::getComponents<ParticleComponent>().size());
            ImGui::Text("       Enemy: %d", Scene::getComponents<EnemyComponent>().size());
            ImGui::Text("  Projectile: %d", Scene::getComponents<ProjectileComponent>().size());
        }
    );

    // Misc
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Misc",
        [&]() {
            /* Light dir */
            glm::vec3 lightDir(RenderSystem::getLightDir());
            ImGui::SliderFloat3("LightDir", glm::value_ptr(lightDir), -1.f, 1.f);
            RenderSystem::setLightDir(lightDir);
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
    
    // Shadows 
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Shadows",
        [&]() {
            /* Light dir */
            glm::vec3 lightDir = RenderSystem::getLightDir();
            ImGui::SliderFloat3("LightDir", glm::value_ptr(lightDir), -1.f, 1.f);
            RenderSystem::setLightDir(lightDir);
            /* Light distance */
            ImGui::SliderFloat("LightPos", &RenderSystem::lightDist, -100.f, 100.f);
            /* Light ortho */
            glm::vec2 hBounds = RenderSystem::s_lightCamera->hBounds();
            glm::vec2 vBounds = RenderSystem::s_lightCamera->vBounds();
            float nPlane = RenderSystem::s_lightCamera->near();
            float fPlane = RenderSystem::s_lightCamera->far();
            float min[2] = { -100.f,   0.f };
            float max[2] = {    0.f, 100.f };
            ImGui::SliderFloat2("H Bounds", glm::value_ptr(hBounds), min, max);
            ImGui::SliderFloat2("V Bounds", glm::value_ptr(vBounds), min, max);
            ImGui::SliderFloat("Near plane", &nPlane, -150.0f, 150.0f);
            ImGui::SliderFloat("Far plane", &fPlane, 100.0f, 300.0f);
            RenderSystem::s_lightCamera->setOrthoBounds(hBounds, vBounds);
            RenderSystem::s_lightCamera->setNearFar(nPlane, fPlane);
            /* Shadow map FBO */
            int mapSize = RenderSystem::s_shadowShader->getMapSize();
            ImGui::SliderInt("Shadow Map Size", &mapSize, 1024, 16384);
            RenderSystem::s_shadowShader->setMapSize(mapSize);
            ImGui::Image((ImTextureID)uintptr_t(RenderSystem::getShadowMap()), ImVec2(256, 256));
        }
    );

    // Toon shading config
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Toon Shading",
        [&]() {
            DiffuseShader * dShader = RenderSystem::s_diffuseShader.get();
            if (ImGui::Button("Toon")) {
                dShader->toggleToon();
            }
            if (dShader->isToon()) {
                float angle = dShader->getSilAngle();
                ImGui::SliderFloat("Silhouette Angle", &angle, 0.f, 1.f);
                dShader->setSilAngle(angle);
                
                int cells = dShader->getCells();
                if (ImGui::SliderInt("Cells", &cells, 1, 15)) {
                    dShader->setCells(cells);
                }

                for (int i = 0; i < cells; i++) {
                    float minBounds[3] = { -1.f,  0.f,  0.f };
                    float maxBounds[3] = {  1.f,  1.f,  1.f };
                    float vals[3];
                    vals[0] = dShader->getCellIntensity(i);
                    vals[1] = dShader->getCellDiffuseScale(i);
                    vals[2] = dShader->getCellSpecularScale(i);
                    ImGui::SliderFloat3(("Cell " + std::to_string(i)).c_str(), vals, minBounds, maxBounds);
                    dShader->setCellIntensity(i, vals[0]);
                    dShader->setCellDiffuseScale(i, vals[1]);
                    dShader->setCellSpecularScale(i, vals[2]);
                }
            }
        }
    );

    // Bounder shader toggle
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Toggle Shaders",
        [&]() {
            if (ImGui::Button("Diffuse")) {
                RenderSystem::s_diffuseShader->toggleEnabled();
            }
            if (ImGui::Button("Shadows")) {
                RenderSystem::s_shadowShader->toggleEnabled();
            }
            if (ImGui::Button("Bounder")) {
                RenderSystem::s_bounderShader->toggleEnabled();
            }
            if (ImGui::Button("Octree")) {
                RenderSystem::s_octreeShader->toggleEnabled();
            }
            if (ImGui::Button("Ray")) {
                RenderSystem::s_rayShader->toggleEnabled();
            }
            if (ImGui::Button("Post Process")) {
                RenderSystem::s_postProcessShader->toggleEnabled();
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