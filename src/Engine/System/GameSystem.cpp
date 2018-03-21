#include "GameSystem.hpp"

#include "glm/gtc/type_ptr.hpp"

#include "Scene/Scene.hpp"
#include "Systems.hpp"
#include "Shaders/Shaders.hpp"
#include "Loader/Loader.hpp"
#include "Util/Util.hpp"
#include "IO/Window.hpp"
#include "EngineApp/EngineApp.hpp"



//==============================================================================
// LIGHTING

const float GameSystem::Lighting::k_defAmbience = 0.3f;
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
const glm::vec3 GameSystem::Player::k_mainHandPosition = glm::vec3(k_width * 0.5f, -k_height * 0.075f, -k_width * 0.75f);

GameObject * GameSystem::Player::gameObject = nullptr;
SpatialComponent * GameSystem::Player::bodySpatial = nullptr;
SpatialComponent * GameSystem::Player::headSpatial = nullptr;
NewtonianComponent * GameSystem::Player::newtonian = nullptr;
CapsuleBounderComponent * GameSystem::Player::bounder = nullptr;
CameraComponent * GameSystem::Player::camera = nullptr;
PlayerControllerComponent * GameSystem::Player::controller = nullptr;
PlayerComponent * GameSystem::Player::playerComp = nullptr;
HealthComponent * GameSystem::Player::health = nullptr;
AmmoComponent * GameSystem::Player::ammo = nullptr;
SpatialComponent * GameSystem::Player::handSpatial = nullptr;
DiffuseRenderComponent * GameSystem::Player::handDiffuse = nullptr;

void GameSystem::Player::init() {
    gameObject = &Scene::createGameObject();
    bodySpatial = &Scene::addComponent<SpatialComponent>(*gameObject, k_position);
    headSpatial = &Scene::addComponent<SpatialComponent>(*gameObject, bodySpatial);
    newtonian = &Scene::addComponent<NewtonianComponent>(*gameObject, false);
    Scene::addComponentAs<GravityComponent, AcceleratorComponent>(*gameObject);
    Scene::addComponent<GroundComponent>(*gameObject);
    Capsule playerCap(glm::vec3(0.0f, -k_height * 0.5f + k_width, 0.0f), k_width, k_height - 2.0f * k_width);
    bounder = &Scene::addComponentAs<CapsuleBounderComponent, BounderComponent>(*gameObject, k_weight, playerCap);
    camera = &Scene::addComponent<CameraComponent>(*gameObject, k_fov, k_near, k_far, headSpatial);
    controller = &Scene::addComponent<PlayerControllerComponent>(*gameObject, k_lookSpeed, k_moveSpeed, k_jumpSpeed, k_sprintSpeed);
    playerComp = &Scene::addComponent<PlayerComponent>(*gameObject);
    health = &Scene::addComponent<HealthComponent>(*gameObject, k_maxHP);
    handSpatial = &Scene::addComponent<SpatialComponent>(*gameObject, k_mainHandPosition, headSpatial);
}

void GameSystem::Player::restore() {
    health->setValue(health->maxValue());
    if (ammo) ammo->setValue(ammo->maxValue());
}

 

//==============================================================================
// ENEMIES

//------------------------------------------------------------------------------
// Basic

const String GameSystem::Enemies::Basic::k_bodyMeshName = "characters/Enemy_Torso.obj";
const String GameSystem::Enemies::Basic::k_headMeshName = "characters/Enemy_Head.obj";
const String GameSystem::Enemies::Basic::k_textureName = "characters/Enemy_Tex.png";
const glm::vec3 GameSystem::Enemies::Basic::k_headPosition = glm::vec3(0.0f, 0.8f, 0.0f);
const bool GameSystem::Enemies::Basic::k_isToon = true;
const glm::vec3 GameSystem::Enemies::Basic::k_scale = glm::vec3(0.75f);
const unsigned int GameSystem::Enemies::Basic::k_weight = 5;
const float GameSystem::Enemies::Basic::k_moveSpeed = 5.0f;
const float GameSystem::Enemies::Basic::k_maxHP = 100.0f;
const float GameSystem::Enemies::Basic::k_meleeDamage = 15.0f;

void GameSystem::Enemies::Basic::create(const glm::vec3 & position, const float moveSpeed, const float health) {
    const Mesh * bodyMesh(Loader::getMesh(k_bodyMeshName));
    const Mesh * headMesh(Loader::getMesh(k_headMeshName));
    const Texture * texture(Loader::getTexture(k_textureName));
    const DiffuseShader * shader();
    ModelTexture modelTex(texture);
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & bodySpatComp(Scene::addComponent<SpatialComponent>(obj, position, k_scale));
    SpatialComponent & headSpatComp(Scene::addComponent<SpatialComponent>(obj, k_headPosition, &bodySpatComp));
    NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj, false));
    GravityComponent & gravComp(Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj));
    BounderComponent & bodyBoundComp(CollisionSystem::addBounderFromMesh(obj, k_weight, *bodyMesh, false, false, true));
    BounderComponent & headBoundComp(CollisionSystem::addBounderFromMesh(obj, k_weight, *headMesh, false, true, false, &headSpatComp));
    PathfindingComponent & pathComp(Scene::addComponent<PathfindingComponent>(obj, *Player::gameObject, moveSpeed, false));
    DiffuseRenderComponent & bodyRenderComp = Scene::addComponent<DiffuseRenderComponent>(
        obj,
        bodySpatComp,
        *bodyMesh,
        modelTex,
        k_isToon,
        glm::vec2(1.0f),
        false
    );
    DiffuseRenderComponent & headRenderComp = Scene::addComponent<DiffuseRenderComponent>(
        obj,
        headSpatComp,
        *headMesh,
        modelTex,
        k_isToon,
        glm::vec2(1.0f),
        false
    );
    HealthComponent & healthComp(Scene::addComponent<HealthComponent>(obj, health));
    EnemyComponent & enemyComp(Scene::addComponentAs<BasicEnemyComponent, EnemyComponent>(obj, k_meleeDamage));
}

void GameSystem::Enemies::Basic::spawn() {
    glm::vec3 dir(-Player::bodySpatial->w());
    dir = Util::safeNorm(glm::vec3(dir.x, 0.0f, dir.z));
    if (dir == glm::vec3()) {
        return;
    }
    auto pair(CollisionSystem::pickHeavy(Ray(Player::bodySpatial->position(), dir), UINT_MAX));
    Intersect & inter(pair.second);
    if (inter.dist > 20.0f) {
        create(Player::bodySpatial->position() + dir * 20.0f, k_moveSpeed, k_maxHP);
    }    
}

//------------------------------------------------------------------------------
// All

void GameSystem::Enemies::killAll() {
    for (EnemyComponent * enemy : s_enemyComponents) {
        Scene::destroyGameObject(enemy->gameObject());
    }
}

void GameSystem::Enemies::enablePathfinding() {
    for (EnemyComponent * comp : s_enemyComponents) {
        GameObject & enemy(comp->gameObject());
        PathfindingComponent * path(enemy.getComponentByType<PathfindingComponent>());
        if (!path) {
            Scene::addComponent<PathfindingComponent>(enemy, *Player::gameObject, GameSystem::Enemies::Basic::k_moveSpeed, false);
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
// WAVES

const Vector<glm::vec3> GameSystem::Wave::k_spawnPoints = {
    glm::vec3(  0.84, -0.54, -37.80),   // door mid 
    glm::vec3( 12.70,  0.08,  11.77),   // door back right
    glm::vec3(-12.08, -0.10,  11.80),   // door back left 
    glm::vec3(-30.14,  5.55,   8.92),   // seating area back left
    glm::vec3( 33.81,  5.28,   0.67),   // seating area back right
    glm::vec3( 11.23,  5.21, -39.39),   // seating area mid back right
    glm::vec3( -6.42,  5.31, -55.36),   // seating area mid front left 
};
int GameSystem::Wave::waveNumber = 0;
float GameSystem::Wave::spawnPeriod = 0.0f;
float GameSystem::Wave::spawnCooldown = 0.0f;
int GameSystem::Wave::enemiesLeft = 0;
float GameSystem::Wave::enemyHealth = 0.0f;
float GameSystem::Wave::enemySpeed = 0.0f;

/* Compute the number of enemies that will be in the current wave */
// Wave 1: 5, Wave 2: 13, Wave 5: 31, Wave 10: 51, Wave 100: 148
int GameSystem::Wave::computeEnemiesInWave() {
    //return 10 + (int)glm::floor(30 * glm::log(waveNumber));
    return int(glm::floor(5.0f + 30.0f * glm::log(float(waveNumber))));
}

/* Return a random spawn point from the list above plus an offset */
glm::vec3 GameSystem::Wave::randomSpawnPoint() {
    return k_spawnPoints[Util::randomUInt(int(k_spawnPoints.size()))] + glm::vec3(Util::random() * 3.f, 0.f, Util::random() * 3.f);
}

/* Compute the max health for enemies in the current wave */
// Extra 10 hp per wave
float GameSystem::Wave::computeEnemyHealth() {
    //return 100.f + float(glm::pow(waveNumber, 3.2f)) / 15.f;
    return 100.0f + float((waveNumber - 1) * 10);
}

/* Compute the max speed for enemies in the current wave */
// Wave 1: 20% player's sprint speed, Wave 2: 40%, Wave 5: 70%, Wave 10: 90%, Wave infinity: 100% 
float GameSystem::Wave::computeEnemySpeed() {
    //return glm::min(Player::k_moveSpeed * 0.95f, waveNumber / 2.f + 2);
    return (1.0f - glm::exp(-0.25f * float(waveNumber))) * Player::k_sprintSpeed;
}

// Time between enemy spawns
// Wave 1: 3s, Wave 2: 2s, Wave 5: 0.8s, Wave 10+: 0.5s
float GameSystem::Wave::computeSpawnTimerMax() {
    return glm::exp(0.5f * (3.0f - waveNumber)) + 0.5f;
}

void GameSystem::Wave::next() {
    ++waveNumber;
    enemiesLeft = computeEnemiesInWave();
    enemySpeed = computeEnemySpeed();
    enemyHealth = computeEnemyHealth();
    spawnPeriod = computeSpawnTimerMax();
    spawnCooldown = Util::random(0.5f, 1.5f) * spawnPeriod;
}

void GameSystem::Wave::spawnAny(float dt) {
    if (!enemiesLeft) {
        return;
    }

    spawnCooldown -= dt;
    if (spawnCooldown <= 0.0f) {
        Enemies::Basic::create(Wave::randomSpawnPoint(), Wave::enemySpeed, Wave::enemyHealth);
        spawnCooldown = Util::random(0.5f, 1.5f) * spawnPeriod;
        --enemiesLeft;
    }
}

void GameSystem::Wave::end() {
    Enemies::killAll();
    enemiesLeft = 0;
}

bool GameSystem::Wave::finished() {
    return enemiesLeft <= 0 && s_enemyComponents.empty();
}

void GameSystem::Wave::restart() {
    Enemies::killAll();
    waveNumber = 0;
    enemiesLeft = 0;
    spawnCooldown = 0.0f;
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
const float GameSystem::Weapons::PizzaSlice::k_damage = 50.0f;
const int GameSystem::Weapons::PizzaSlice::k_ammo = 30;

GameObject * GameSystem::Weapons::PizzaSlice::fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel, const glm::quat & orient) {
    const Mesh * mesh(Loader::getMesh(k_meshName));
    const Texture * tex(Loader::getTexture(k_texName));
    ModelTexture modelTex(tex);
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, initPos, k_scale, orient));
    BounderComponent & bounderComp(CollisionSystem::addBounderFromMesh(obj, k_weight, *mesh, false, true, false));
    NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj, true));
    GroundComponent & groundComp(Scene::addComponent<GroundComponent>(obj));
    newtComp.addVelocity(initDir * k_speed + srcVel);
    DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj,
        spatComp,
        *mesh,
        modelTex,
        k_isToon,
        glm::vec2(1.0f),
        false
    ));
    ProjectileComponent & weaponComp(Scene::addComponentAs<BulletComponent, ProjectileComponent>(obj, Player::gameObject, k_damage));
    SpinAnimationComponent & spinAnimation(Scene::addComponentAs<SpinAnimationComponent, AnimationComponent>(obj, spatComp, glm::vec3(0.0f, 1.0f, 0.0f), -5.0f));

    SoundSystem::playSound3D("splash2.wav", spatComp.position());

    return &obj;
}

GameObject * GameSystem::Weapons::PizzaSlice::fireFromPlayer() {
    if (!Player::ammo || Player::ammo->value() < 0.5f) {
        return nullptr;
    }
    Player::ammo->changeValue(-1.0f);
    glm::vec3 initPos(Player::handSpatial->position());
    return fire(initPos, Player::camera->getLookDir(), Player::handSpatial->effectiveVelocity(), Player::handSpatial->orientation());
}

void GameSystem::Weapons::PizzaSlice::equip() {
    const Mesh * mesh(Loader::getMesh("weapons/Pizza_Slice.obj"));
    const Texture * tex(Loader::getTexture("weapons/Pizza_Tex.png"));
    ModelTexture modelTex(tex);
    Player::handDiffuse = &Scene::addComponent<DiffuseRenderComponent>(*Player::gameObject,
        *Player::handSpatial,
        *mesh,
        modelTex,
        true,
        glm::vec2(1.0f),
        false   
    );
    Player::ammo = &Scene::addComponent<AmmoComponent>(*Player::gameObject, float(k_ammo));
}

void GameSystem::Weapons::PizzaSlice::unequip() {
    if (Player::handDiffuse) {
        Scene::removeComponent(*Player::handDiffuse);
        Player::handDiffuse = nullptr;
    }
    if (Player::ammo) {
        Scene::removeComponent(*Player::ammo);
        Player::ammo = nullptr;
    }
}

//------------------------------------------------------------------------------
// Soda Grenade

const String GameSystem::Weapons::SodaGrenade::k_meshName = "weapons/SodaCan.obj";
const String GameSystem::Weapons::SodaGrenade::k_texName = "weapons/SodaCan_Tex.png";
const bool GameSystem::Weapons::SodaGrenade::k_isToon = true;
const glm::vec3 GameSystem::Weapons::SodaGrenade::k_scale = glm::vec3(1.0f);
const unsigned int GameSystem::Weapons::SodaGrenade::k_weight = 1;
const float GameSystem::Weapons::SodaGrenade::k_speed = 20.0f; 
const float GameSystem::Weapons::SodaGrenade::k_damage = 50.0f;
const float GameSystem::Weapons::SodaGrenade::k_radius = 5.0f;
const int GameSystem::Weapons::SodaGrenade::k_ammo = 15;

GameObject * GameSystem::Weapons::SodaGrenade::fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel) {
    const Mesh * mesh(Loader::getMesh(k_meshName));
    const Texture * tex(Loader::getTexture(k_texName));
    ModelTexture modelTex(tex);
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, initPos, k_scale, Player::headSpatial->orientation()));
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
        glm::vec2(1.0f),
        false
    ));
    ProjectileComponent & weaponComp(Scene::addComponentAs<GrenadeComponent, ProjectileComponent>(obj, Player::gameObject, k_damage, k_radius));
    SpinAnimationComponent & spinAnimation(Scene::addComponentAs<SpinAnimationComponent, AnimationComponent>(obj, spatComp, glm::vec3(1.0f, 0.0f, 0.0f), -5.0f));

    SoundSystem::playSound3D("splash3.wav", spatComp.position());

    return &obj;
}

GameObject * GameSystem::Weapons::SodaGrenade::fireFromPlayer() {
    if (!Player::ammo || Player::ammo->value() < 0.5f) {
        return nullptr;
    }
    Player::ammo->changeValue(-1.0f);
    glm::vec3 initPos(Player::headSpatial->orientMatrix() * Player::k_mainHandPosition + Player::headSpatial->position());
    return fire(initPos, Player::camera->getLookDir(), Player::handSpatial->effectiveVelocity());
}

void GameSystem::Weapons::SodaGrenade::equip() {
    const Mesh * mesh(Loader::getMesh("weapons/SodaCan.obj"));
    const Texture * tex(Loader::getTexture("weapons/SodaCan_Tex.png"));
    ModelTexture modelTex(tex);
    Player::handDiffuse = &Scene::addComponent<DiffuseRenderComponent>(*Player::gameObject,
        *Player::handSpatial,
        *mesh,
        modelTex,
        true,
        glm::vec2(1.0f),
        false   
    );
    Player::ammo = &Scene::addComponent<AmmoComponent>(*Player::gameObject, float(k_ammo));
}

void GameSystem::Weapons::SodaGrenade::unequip() {
    if (Player::handDiffuse) {
        Scene::removeComponent(*Player::handDiffuse);
        Player::handDiffuse = nullptr;
    }
    if (Player::ammo) {
        Scene::removeComponent(*Player::ammo);
        Player::ammo = nullptr;
    }
}

//------------------------------------------------------------------------------
// Sriracha Bottle

const float GameSystem::Weapons::SrirachaBottle::k_damage = 100.0f;
const float GameSystem::Weapons::SrirachaBottle::k_radius = 1.5f;
const float GameSystem::Weapons::SrirachaBottle::k_ammo = 10.0f; // seconds

GameObject * GameSystem::Weapons::SrirachaBottle::playerSriracha;

GameObject * GameSystem::Weapons::SrirachaBottle::start(const SpatialComponent & hostSpatial, const glm::vec3 & offset) {
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, hostSpatial.position(), glm::vec3(1.0f), hostSpatial.orientation()));
    BounderComponent & bounderComp(Scene::addComponentAs<SphereBounderComponent, BounderComponent>(obj, 0, Sphere(glm::vec3(0.0f, 0.0f, -k_radius), k_radius)));
    SprayComponent & weaponComp(Scene::addComponentAs<SprayComponent, MeleeComponent>(obj, &hostSpatial, offset, k_damage));
    ParticleComponent & particleComp(ParticleSystem::addSrirachaPC(spatComp));

    SoundSystem::playSound3D("sword_slash.wav", spatComp.position());

    return &obj;
}

void GameSystem::Weapons::SrirachaBottle::toggleForPlayer() {
    if (playerSriracha) {
        Scene::destroyGameObject(*playerSriracha);
        playerSriracha = nullptr;
        Player::handSpatial->setRelativeScale(glm::vec3(1.0f));
    }
    else {
        playerSriracha = start(*Player::handSpatial);
        Scene::addComponentAs<ScaleToAnimationComponent, AnimationComponent>(*playerSriracha, *Player::handSpatial, glm::vec3(1.0f, 0.5f, 1.0f), 5.0f);
    }
}

void GameSystem::Weapons::SrirachaBottle::updatePlayers(float dt) {
    if (!Player::ammo || Player::ammo->value() < 0.5f) {
        return;
    }
    Player::ammo->changeValue(-dt);
}

void GameSystem::Weapons::SrirachaBottle::equip() {
    const Mesh * mesh(Loader::getMesh("weapons/Sriracha.obj"));
    const Texture * tex(Loader::getTexture("weapons/Sriracha_Tex.png"));
    ModelTexture modelTex(tex);
    Player::handDiffuse = &Scene::addComponent<DiffuseRenderComponent>(*Player::gameObject,
        *Player::handSpatial,
        *mesh,
        modelTex,
        true,
        glm::vec2(1.0f),
        false   
    );

    Player::ammo = &Scene::addComponent<AmmoComponent>(*Player::gameObject, float(k_ammo));
}

void GameSystem::Weapons::SrirachaBottle::unequip() {
    if (Player::handDiffuse) {
        Scene::removeComponent(*Player::handDiffuse);
        Player::handDiffuse = nullptr;
    }
    if (Player::ammo) {
        Scene::removeComponent(*Player::ammo);
        Player::ammo = nullptr;
    }

    if (playerSriracha) toggleForPlayer();
}

//------------------------------------------------------------------------------
// All

void GameSystem::Weapons::destroyAllWeapons() {        
    for (ProjectileComponent * comp : s_projectileComponents) {
        Scene::destroyGameObject(comp->gameObject());
    }
    for (BlastComponent * comp : s_blastComponents) {
        Scene::destroyGameObject(comp->gameObject());
    }
    for (MeleeComponent * comp : s_meleeComponents) {
        Scene::destroyGameObject(comp->gameObject());
    }
}



//==============================================================================
// SHOPS

//------------------------------------------------------------------------------
// American

BounderComponent * GameSystem::Shops::American::bounder = nullptr;
bool GameSystem::Shops::American::isOpen = true;

void GameSystem::Shops::American::init() {
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatial(Scene::addComponent<SpatialComponent>(obj));
    bounder = &Scene::addComponentAs<AABBounderComponent, BounderComponent>(obj, 0, AABox(glm::vec3(-38.0f, -2.0f, -85.5f), glm::vec3(-36.0f, 1.0f, -73.5f)));

    auto collisionCallback([&](const Message & msg_) {
        const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));
        if (&msg.bounder1 == bounder && &msg.bounder2 == static_cast<const BounderComponent *>(Player::bounder)) {
            s_shopVisited = Culture::american;
        }
    });
    Scene::addReceiver<CollisionMessage>(&obj, collisionCallback);
}

void GameSystem::Shops::American::open() {
    if (isOpen) return;
    SoundSystem::playSound3D("cash_register.wav", bounder->center());
    isOpen = true;
}

void GameSystem::Shops::American::close() {    
    isOpen = false;
}

//------------------------------------------------------------------------------
// Asian

BounderComponent * GameSystem::Shops::Asian::bounder = nullptr;
bool GameSystem::Shops::Asian::isOpen = true;

void GameSystem::Shops::Asian::init() {
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatial(Scene::addComponent<SpatialComponent>(obj));
    bounder = &Scene::addComponentAs<AABBounderComponent, BounderComponent>(obj, 0, AABox(glm::vec3(-14.0f, -2.0f, -172.0f), glm::vec3(-12.0f, 1.0f, -161.0f)));

    auto collisionCallback([&](const Message & msg_) {
        const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));
        if (&msg.bounder1 == bounder && &msg.bounder2 == static_cast<const BounderComponent *>(Player::bounder)) {
            s_shopVisited = Culture::asian;
        }
    });
    Scene::addReceiver<CollisionMessage>(&obj, collisionCallback);
}

void GameSystem::Shops::Asian::open() {
    if (isOpen) return;
    SoundSystem::playSound3D("cash_register.wav", bounder->center());
    isOpen = true;
}

void GameSystem::Shops::Asian::close() {    
    isOpen = false;
}

//------------------------------------------------------------------------------
// Italian

BounderComponent * GameSystem::Shops::Italian::bounder = nullptr;
bool GameSystem::Shops::Italian::isOpen = true;

void GameSystem::Shops::Italian::init() {
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spatial(Scene::addComponent<SpatialComponent>(obj));
    bounder = &Scene::addComponentAs<AABBounderComponent, BounderComponent>(obj, 0, AABox(glm::vec3(38.0f, -2.0f, -46.0f), glm::vec3(40.5f, 1.0f, -34.5f)));

    auto collisionCallback([&](const Message & msg_) {
        const CollisionMessage & msg(static_cast<const CollisionMessage &>(msg_));
        if (&msg.bounder1 == bounder && &msg.bounder2 == static_cast<const BounderComponent *>(Player::bounder)) {
            s_shopVisited = Culture::italian;
        }
    });
    Scene::addReceiver<CollisionMessage>(&obj, collisionCallback);
}

void GameSystem::Shops::Italian::open() {
    if (isOpen) return;
    SoundSystem::playSound("cash_register.wav");
    isOpen = true;
}

void GameSystem::Shops::Italian::close() {    
    isOpen = false;
}

//------------------------------------------------------------------------------
// All

const float GameSystem::Shops::k_rotationPeriod = 30.0f; // new shop opens every 30 seconds

float GameSystem::Shops::rotationCooldown = k_rotationPeriod;

void GameSystem::Shops::init() {
    American::init();
    Asian::init();
    Italian::init();
}

void GameSystem::Shops::update(float dt) {
    if (numAbleToOpen()) {
        rotationCooldown -= dt;
        if (rotationCooldown <= 0.0f) {
            openRandom();
            rotationCooldown = k_rotationPeriod;
        }
    }
    else {
        rotationCooldown = k_rotationPeriod;
    }
}

void GameSystem::Shops::openAll() {
    American::open();
    Asian::open();
    Italian::open();
}

void GameSystem::Shops::openRandom() {
    int n(numAbleToOpen());
    if (n == 0) return;
    int i(std::rand() % n);
    if (American::isOpen || s_culture == Culture::american) ++i;
    else if (i == 0) { American::open(); return; }
    if (Asian::isOpen || s_culture == Culture::asian) ++i;
    else if (i == 1) { Asian::open(); return; }
    if (Italian::isOpen || s_culture == Culture::italian) ++i;
    else if (i == 2) { Italian::open(); return; }
}

int GameSystem::Shops::numOpen() {
    return int(American::isOpen) + int(Asian::isOpen) + int(Italian::isOpen);
}

int GameSystem::Shops::numClosed() {
    return 3 - numOpen();
}

int GameSystem::Shops::numAbleToOpen() {
    return
        int(!American::isOpen && s_culture != Culture::american),
        int(!Asian::isOpen && s_culture != Culture::asian),
        int(!Italian::isOpen && s_culture != Culture::italian);
}

bool GameSystem::Shops::isOpen(Culture shop) {
    switch (shop) {
        case Culture::american: return American::isOpen;
        case Culture::asian: return Asian::isOpen;
        case Culture::italian: return Italian::isOpen;
    }
    return false;
}

void GameSystem::Shops::close(Culture shop) {
    switch (shop) {
        case Culture::american: American::close(); return;
        case Culture::asian: Asian::close(); return;
        case Culture::italian: Italian::close(); return;
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
const String GameSystem::Music::k_american = "bgRock1.mp3";
const String GameSystem::Music::k_asian = "bgAsianStereotype1.mp3";
const String GameSystem::Music::k_italian = "bgItalian2.mp3";

bool GameSystem::Music::s_playing = false;

void GameSystem::Music::start() {
    if (s_playing) return;
    SoundSystem::playBackgroundMusic();
    SoundSystem::setBackgroundMusicVolume(0.05f);
    s_playing = true;
}

void GameSystem::Music::stop() {
    if (!s_playing) return;
    SoundSystem::pauseBackgroundMusic();
    s_playing = false;
}

void GameSystem::Music::toggle() {
    s_playing = !s_playing;
    if (s_playing) start();
    else stop();
}

void GameSystem::Music::set() {
    stop();
    switch (s_culture) {
        case Culture::american: SoundSystem::setBackgroundMusic(k_american, true); break;
        case Culture::asian: SoundSystem::setBackgroundMusic(k_asian, true); break;
        case Culture::italian: SoundSystem::setBackgroundMusic(k_italian, true); break;
        case Culture::none: return;
    }
    start();
}



//==============================================================================
// Game System

const Vector<CameraComponent *> & GameSystem::s_cameraComponents(Scene::getComponents<CameraComponent>());
const Vector<CameraControllerComponent *> & GameSystem::s_cameraControllerComponents(Scene::getComponents<CameraControllerComponent>());
const Vector<PlayerControllerComponent *> & GameSystem::s_playerControllers(Scene::getComponents<PlayerControllerComponent>());
const Vector<PlayerComponent *> & GameSystem::s_playerComponents(Scene::getComponents<PlayerComponent>());
const Vector<EnemyComponent *> & GameSystem::s_enemyComponents(Scene::getComponents<EnemyComponent>());
const Vector<ProjectileComponent *> & GameSystem::s_projectileComponents(Scene::getComponents<ProjectileComponent>());
const Vector<BlastComponent *> & GameSystem::s_blastComponents(Scene::getComponents<BlastComponent>());
const Vector<MeleeComponent *> & GameSystem::s_meleeComponents(Scene::getComponents<MeleeComponent>());
 
const glm::vec3 GameSystem::k_defGravity = glm::vec3(0.0f, -10.0f, 0.0f);
const float GameSystem::k_restTime = 8.0f;
const float GameSystem::k_purgatoryTime = 16.0f;

GameSystem::Culture GameSystem::s_culture = Culture::none;
bool GameSystem::s_changeCulture = false;
GameSystem::Culture GameSystem::s_newCulture = Culture::none;
bool GameSystem::s_useWeapon = false;
bool GameSystem::s_unuseWeapon = false;
GameSystem::Culture GameSystem::s_shopVisited = Culture::none;
bool GameSystem::s_resting = true;
float GameSystem::s_restCooldown = k_restTime;
bool GameSystem::s_inPurgatory = true;
float GameSystem::s_purgatoryCooldown = 0.0f;
bool GameSystem::s_playerDied = false;

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

    // Set sound ear
    SoundSystem::setEar(*Player::headSpatial);

    // Load Level
    Loader::loadLevel(EngineApp::RESOURCE_DIR + "GameLevel_03.json");
    // Set octree. Needs to be manually adjusted to fit level size
    CollisionSystem::setOctree(glm::vec3(-70.0f, -10.0f, -210.0f), glm::vec3(70.0f, 50.0f, 40.0f), 1.0f);

    // Init Shops
    Shops::init();

    // Set message callbacks
    setupMessageCallbacks();
    
    // Setup ImGui
    setupImGui();

    // Water fountain
    GameObject & fountain(Scene::createGameObject());
    SpatialComponent & fountainSpat(Scene::addComponent<SpatialComponent>(fountain, glm::vec3(1.0f, 7.6f, -51.0f)));
    fountainSpat.setRelativeUVW(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), true);
    ParticleSystem::addWaterFountainPC(fountainSpat);
    SoundSystem::playSound3D("water_fountain.wav", fountainSpat.position(), true);

}

void GameSystem::update(float dt) {
    // Update controllers (must be before game logic)
    for (auto & comp : s_playerControllers) {
        comp->update(dt);
    }
    for (auto & comp : s_cameraControllerComponents) {
        comp->update(dt);
    }

    // Game Logic
    updateGame(dt);

    // Update components
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
    for (auto & comp : s_meleeComponents) {
        comp->update(dt);
    }
}

void GameSystem::updateGame(float dt) {
    // Player dead, or dying, or whatever. Time between games.
    if (s_inPurgatory) {
        // Still in purgatory
        if ((s_purgatoryCooldown -= dt) > 0.0f) {
            RenderSystem::s_postProcessShader->setScreenTone(glm::vec3(s_purgatoryCooldown / k_purgatoryTime));
            return;
        }
        // Out of purgatory, lets go
        RenderSystem::s_postProcessShader->setScreenTone(glm::vec3(1.0f));
        startGame();
        return;
    }

    // Player died
    if (s_playerDied) {
        s_playerDied = false;
        endGame();
        return;
    }

    // Player uses weapon
    if (s_useWeapon) {
        switch (s_culture) {
            case Culture::american: Weapons::SodaGrenade::fireFromPlayer(); break;
            case Culture::asian: Weapons::SrirachaBottle::toggleForPlayer(); break;
            case Culture::italian: Weapons::PizzaSlice::fireFromPlayer(); break;
        }
        s_useWeapon = false;
    }
    else if (s_unuseWeapon) {
        if (s_culture == Culture::asian) Weapons::SrirachaBottle::toggleForPlayer();
        s_unuseWeapon = false;
    }
    // Extra logic for depleting sriracha ammo
    if (Weapons::SrirachaBottle::playerSriracha && Mouse::isDown(0)) {
        Weapons::SrirachaBottle::updatePlayers(dt);
    }

    // Player runs out of ammo
    if (Player::ammo && Player::ammo->value() < 0.5f) {
        s_changeCulture = true;
        s_newCulture = Culture::none;
    }

    // Update shops
    Shops::update(dt);

    // Player visits shop
    if (s_shopVisited != Culture::none && s_shopVisited != s_culture && Shops::isOpen(s_shopVisited)) {
        Shops::close(s_shopVisited);
        s_changeCulture = true;
        s_newCulture = s_shopVisited;
    }
    s_shopVisited = Culture::none;

    // Change culture
    if (s_changeCulture) {
        setCulture(s_newCulture);
        s_changeCulture = false;
    }

    // In between waves
    if (s_resting) {
        s_restCooldown -= dt;
        // Rest over
        if (s_restCooldown <= 0.0f) {
            startWave();
            s_resting = false;
        }
    }
    // Wave ongoing
    else {
        // Wave finished
        if (Wave::finished()) {
            endWave();
            s_resting = true;
            s_restCooldown = k_restTime;
        }
        else {
            Wave::spawnAny(dt);
        }
    }
}

void GameSystem::startGame() {
    setCulture(Culture::none);
    Player::bodySpatial->setRelativePosition(Player::k_playerPosition);
    Player::restore();
    Wave::restart();
    Weapons::destroyAllWeapons();
    Shops::openAll();
    s_inPurgatory = false;
    s_resting = true;
    s_restCooldown = k_restTime;
}

void GameSystem::endGame() {
    setCulture(Culture::none);
    Enemies::killAll();
    SoundSystem::playSound("player_death.wav");
    s_inPurgatory = true;
    s_purgatoryCooldown = k_purgatoryTime;
}

void GameSystem::setCulture(Culture culture) {
    unequipWeapon();

    s_culture = culture;

    equipWeapon();
    Music::set();
}

void GameSystem::equipWeapon() {
    switch (s_culture) {
        case Culture::american: Weapons::SodaGrenade::equip(); break;
        case Culture::asian: Weapons::SrirachaBottle::equip(); break;
        case Culture::italian: Weapons::PizzaSlice::equip(); break;
    }
}

void GameSystem::unequipWeapon() {
    switch (s_culture) {
        case Culture::american: Weapons::SodaGrenade::unequip(); break;
        case Culture::asian: Weapons::SrirachaBottle::unequip(); break;
        case Culture::italian: Weapons::PizzaSlice::unequip(); break;
    }
}

void GameSystem::startWave() {
    SoundSystem::playSound("gong.mp3");
    Wave::next();
}

void GameSystem::endWave() {
    Player::restore();
    Shops::openAll();
}



//==============================================================================
// Message Handling

void GameSystem::setupMessageCallbacks() {
    // Use weapon (click)
    auto useWeaponCallback([&](const Message & msg_) {
        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && !(msg.mods & GLFW_MOD_CONTROL)) {
            if (msg.action == GLFW_PRESS) {
                s_useWeapon = true;                
            }
            else if (msg.action == GLFW_RELEASE) {
                s_unuseWeapon = true;
            }
        }
    });
    Scene::addReceiver<MouseMessage>(nullptr, useWeaponCallback);

    // Player death
    auto playerDeathCallback([&](const Message & msg_) {
        if (!s_inPurgatory) s_playerDied = true;
    });
    Scene::addReceiver<PlayerDeathMessage>(nullptr, playerDeathCallback);
    
    // Set culture (1 | 2 | 3 | 4)
    auto setCultureCallback([&](const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.action != GLFW_PRESS || msg.mods) {
            return;
        }
        switch (msg.key) {
            case GLFW_KEY_1: { s_changeCulture = true; s_newCulture = Culture::american; break; }
            case GLFW_KEY_2: { s_changeCulture = true; s_newCulture = Culture::asian; break; }
            case GLFW_KEY_3: { s_changeCulture = true; s_newCulture = Culture::italian; break; }
            case GLFW_KEY_4: { s_changeCulture = true; s_newCulture = Culture::none; break; }
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, setCultureCallback);

    // Game Controls
    auto gameControlsCallback([&](const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.action != GLFW_PRESS) {
            return;
        }
        
        // Spawn Enemy (enter)
        if (msg.key == GLFW_KEY_ENTER) {
            Enemies::Basic::spawn();
        }
        // Kill all enemies (delete)
        else if (msg.key == GLFW_KEY_DELETE) {
            Enemies::killAll();
        }
        // End wave or end rest (backspace)
        else if (msg.key == GLFW_KEY_BACKSPACE) {
            if (s_resting) {
                s_restCooldown = 0.0f;
            }
            else {
                Wave::end();
            }
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, gameControlsCallback);

    // Shoot ray (ctrl-click)
    auto rayPickCallback([&](const Message & msg_) {
        static const int rayDepth(100);
        static Vector<glm::vec3> rayPositions;

        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && msg.mods & GLFW_MOD_CONTROL && msg.action == GLFW_PRESS) {
            rayPositions.clear();
            rayPositions.push_back(Player::headSpatial->position());
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
                Freecam::spatialComp->setRelativePosition(Player::headSpatial->position());
                Freecam::controllerComp->setOrientation(Player::controller->theta(), Player::controller->phi());
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
                Player::bodySpatial->position().x,
                Player::bodySpatial->position().y,
                Player::bodySpatial->position().z
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
            ImGui::Text("      Particle: %5.2f%%, %5.2f%%", Scene::     particleDT * factor, Scene::              particleDT * factor);
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
    
    // Game stats
    Scene::addComponent<ImGuiComponent>(
        imguiGO, 
        "Game Stats",
        [&]() {
            ImGui::Text("Wave number: %d", Wave::waveNumber);
            ImGui::Text("Enemies still to spawn: %d", Wave::enemiesLeft);
            ImGui::Text("Enemies alive: %d", s_enemyComponents.size());
            ImGui::Text("Enemy health: %5.2f", Wave::enemyHealth);
            ImGui::Text("Enemy speed: %5.2f", Wave::enemySpeed);
            ImGui::Text("Enemy spawn period: %5.2f", Wave::spawnPeriod);
            ImGui::Text("Enemy spawn cooldown: %5.2f", Wave::spawnCooldown);
            ImGui::Text("Resting: %d", s_resting);
            ImGui::Text("Rest time left: %5.2f", s_restCooldown);
       }
    );

    // Misc
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Misc",
        [&]() {
            /* VSync */
            if (ImGui::Button("VSync")) {
                Window::toggleVSync();
            }
            /* Path finding */
            if (ImGui::Button("Turn off Path finding")) {
                Enemies::disablePathfinding();
            }
            ImGui::SliderFloat2("Health Size", glm::value_ptr(RenderSystem::s_healthShader->m_size), 0.f, 1.f);
        }
    );

    // Shadows 
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Shadows",
        [&]() {
            /* Light dir */
            glm::vec3 lightDir = RenderSystem::getLightDir();
            if (ImGui::SliderFloat3("LightDir", glm::value_ptr(lightDir), -1.f, 1.f)) {
                RenderSystem::setLightDir(lightDir);
            }
            /* Shadow map's distance */
            ImGui::SliderFloat("Shadow Distance", &RenderSystem::lightDist, -350.f, 350.f);
            ImGui::SliderFloat("Shadow Offset", &RenderSystem::lightOffset, 0.f, 50.f);
            ImGui::SliderFloat("Shadow Ambience", &RenderSystem::shadowAmbience, 0.f, 1.f);
            ImGui::SliderFloat("Shadow Transition", &RenderSystem::transitionDistance, 0.f, 50.f);
            /* Shadow map FBO */
            int mapSize = RenderSystem::s_shadowShader->getMapSize();
            if (ImGui::SliderInt("Shadow Map Size", &mapSize, 1024, 16384)) {
                RenderSystem::s_shadowShader->setMapSize(mapSize);
            }
            ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(RenderSystem::getShadowMap()->textureId)), ImVec2(256, 256));

            glm::vec3 position = RenderSystem::s_lightSpatial->position();
            if (ImGui::SliderFloat3("Position", glm::value_ptr(position), -300.f, 300.f)) {
                RenderSystem::s_lightSpatial->setRelativePosition(position);
            }
            glm::vec2 xBounds = RenderSystem::s_lightCamera->hBounds();
            glm::vec2 yBounds = RenderSystem::s_lightCamera->vBounds();
            glm::vec2 zBounds = glm::vec2(RenderSystem::s_lightCamera->near(), RenderSystem::s_lightCamera->far());
            if (ImGui::SliderFloat2("xBounds", glm::value_ptr(xBounds), -300.f, 300.f) ||
                ImGui::SliderFloat2("yBounds", glm::value_ptr(yBounds), -300.f, 300.f)) {
                RenderSystem::s_lightCamera->setOrthoBounds(xBounds, yBounds);
            }
            if (ImGui::SliderFloat2("zBounds", glm::value_ptr(zBounds), -300.f, 300.f)) {
                RenderSystem::s_lightCamera->setNearFar(zBounds.x, zBounds.y);
            }
            ImGui::SliderInt("PCF", &RenderSystem::pcfCount, 0, 10);
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
