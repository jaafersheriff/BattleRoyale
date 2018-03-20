#pragma once
#ifndef _GAME_SYSTEM_HPP_
#define _GAME_SYSTEM_HPP_



#include "System.hpp"
#include "Model/Material.hpp"



class SpatialComponent;
class NewtonianComponent;
class CapsuleBounderComponent;
class CameraComponent;
class PlayerControllerComponent;
class PlayerComponent;
class HealthComponent;
class CameraControllerComponent;
class ImGuiComponent;
class EnemyComponent;
class ProjectileComponent;
class BlastComponent;
struct GameInterface;



// static class
class GameSystem {

    friend Scene;
    friend GameInterface;

    private:

    //--------------------------------------------------------------------------
    // Waves 

    struct Wave {
        static const Vector<glm::vec3> k_spawnPoints;
        static int k_waveNumber;
        static float k_spawnTimer;
        static float k_spawnTimerMax;
        static int k_enemiesAlive;
        static int k_enemiesInWave;
        static float k_enemyHealth;
        static float k_enemySpeed;

        static glm::vec3 randomSpawnPoint();
        static int computeEnemiesInWave();
        static float computeEnemyHealth();
        static float computeEnemySpeed();
    };

    //--------------------------------------------------------------------------
    // Lighting

    struct Lighting {

        static const float k_defAmbience;
        static const Material k_defMaterial;
        static const glm::vec3 k_defLightDir;

    };

    //--------------------------------------------------------------------------
    // Player

    struct Player {

        static const glm::vec3 k_position;
        static const float k_height;
        static const float k_width;
        static const unsigned int k_weight;
        static const float k_lookSpeed;
        static const float k_moveSpeed;
        static const float k_sprintSpeed;
        static const float k_jumpSpeed;
        static const float k_fov;
        static const float k_near;
        static const float k_far;
        static const float k_maxHP;
        static const glm::vec3 k_playerPosition;
        static const glm::vec3 k_mainHandPosition;

        static GameObject * gameObject;
        static SpatialComponent * spatial;
        static NewtonianComponent * newtonian;
        static CapsuleBounderComponent * bounder;
        static CameraComponent * camera;
        static PlayerControllerComponent * controller;
        static PlayerComponent * playerComp;
        static HealthComponent * health;
        static SpatialComponent * handSpatial;

        static void init();

    };

    //--------------------------------------------------------------------------
    // Enemies

    struct Enemies {

        struct Basic {

            static const String k_bodyMeshName;
            static const String k_headMeshName;
            static const String k_textureName;
            static const glm::vec3 k_headPosition;
            static const bool k_isToon;
            static const glm::vec3 k_scale;
            static const unsigned int k_weight;
            static const float k_moveSpeed;
            static const float k_maxHP;

            static void create(const glm::vec3 & position, float speed, float hp);

            static void spawn();

        };

        static void enablePathfinding();

        static void disablePathfinding();

    };

    //--------------------------------------------------------------------------
    // Weapons

    struct Weapons {

        struct PizzaSlice {

            static const String k_meshName;
            static const String k_texName;
            static const bool k_isToon;
            static const glm::vec3 k_scale;
            static const unsigned int k_weight;
            static const float k_speed; 
            static const float k_damage;

            static void fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel);
            static void fireFromPlayer();

        };

        struct SodaGrenade {

            static const String k_meshName;
            static const String k_texName;
            static const bool k_isToon;
            static const glm::vec3 k_scale;
            static const unsigned int k_weight;
            static const float k_speed; 
            static const float k_damage;
            static const float k_radius;

            static void fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel);
            static void fireFromPlayer();

        };

        static void destroyAllProjectiles();

    };

    //--------------------------------------------------------------------------
    // Freecam

    struct Freecam {

        static const float k_minSpeed;
        static const float k_maxSpeed;

        static GameObject * gameObject;
        static SpatialComponent * spatialComp;
        static CameraComponent * cameraComp;
        static CameraControllerComponent * controllerComp;

        static void init();

    };

    //--------------------------------------------------------------------------
    // Music

    struct Music {

        static const String k_defMusic;

        static bool s_playing;

        static void stop();

        static void start();

        static void toggle();

    };
    
    //--------------------------------------------------------------------------

    public:

    static void init();

    static void update(float dt);

    private:

    static void setupMessageCallbacks();

    static void setupImGui();

    private:

    static const glm::vec3 k_defGravity;

    static const Vector<CameraComponent *> & s_cameraComponents;
    static const Vector<CameraControllerComponent *> & s_cameraControllerComponents;
    static const Vector<PlayerControllerComponent *> & s_playerControllers;
    static const Vector<ImGuiComponent *> & s_imguiComponents;
    static const Vector<PlayerComponent *> & s_playerComponents;
    static const Vector<EnemyComponent *> & s_enemyComponents;
    static const Vector<ProjectileComponent *> & s_projectileComponents;
    static const Vector<BlastComponent *> & s_blastComponents;

};

#endif