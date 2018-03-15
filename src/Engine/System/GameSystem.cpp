#include "GameSystem.hpp"

#include "Scene/Scene.hpp"



//==============================================================================
// LIGHTING

const float GameSystem::Lighting::k_defAmbience = 0.3f;
const glm::vec3 GameSystem::Lighting::k_defLightDir = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));



//==============================================================================
// PLAYER

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

void GameSystem::Player::init(const glm::vec3 & position) {
    gameObject = &Scene::createGameObject();
    spatial = &Scene::addComponent<SpatialComponent>(*gameObject, position);
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


 
const glm::vec3 GameSystem::k_defGravity = glm::vec3(0.0f, -10.0f, 0.0f);

const Vector<CameraComponent *> & GameSystem::s_cameraComponents(Scene::getComponents<CameraComponent>());
const Vector<CameraControllerComponent *> & GameSystem::s_cameraControllerComponents(Scene::getComponents<CameraControllerComponent>());
const Vector<PlayerControllerComponent *> & GameSystem::s_playerControllers(Scene::getComponents<PlayerControllerComponent>());
const Vector<PlayerComponent *> & GameSystem::s_playerComponents(Scene::getComponents<PlayerComponent>());
const Vector<EnemyComponent *> & GameSystem::s_enemyComponents(Scene::getComponents<EnemyComponent>());
const Vector<ProjectileComponent *> & GameSystem::s_projectileComponents(Scene::getComponents<ProjectileComponent>());
const Vector<BlastComponent *> & GameSystem::s_blastComponents(Scene::getComponents<BlastComponent>());

void GameSystem::init(const glm::vec3 & playerPosition) {

    // Set gravity
    SpatialSystem::setGravity(k_defGravity);

    // Init player
    Player::init(playerPosition);

    // Setup Free Cam
    Freecam::init();

    // Set primary camera
    RenderSystem::setCamera(Player::camera);

    // Set Sound camera
    SoundSystem::setCamera(Player::camera);
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