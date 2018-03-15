#pragma once
#ifndef _GAME_SYSTEM_HPP_
#define _GAME_SYSTEM_HPP_



#include "System.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/CameraComponents/CameraControllerComponent.hpp"
#include "Component/PlayerComponents/PlayerControllerComponent.hpp"
#include "Component/ImGuiComponents/ImGuiComponent.hpp"
#include "Component/PlayerComponents/PlayerComponent.hpp"
#include "Component/EnemyComponents/EnemyComponent.hpp"
#include "Component/WeaponComponents/ProjectileComponents.hpp"
#include "Component/WeaponComponents/BlastComponent.hpp"



// static class
class GameSystem {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::game;

    private:

    //--------------------------------------------------------------------------
    // Lighting

    struct Lighting {

        static const float k_defAmbience;
        static const glm::vec3 k_defLightDir;

    };

    //--------------------------------------------------------------------------
    // Player

    struct Player {

        static const float k_defHeight;
        static const float k_defWidth;
        static const unsigned int k_defWeight;
        static const float k_defLookSpeed;
        static const float k_defMoveSpeed;
        static const float k_defSprintSpeed;
        static const float k_defJumpSpeed;
        static const float k_defFOV;
        static const float k_defNear;
        static const float k_defFar;
        static const float k_defMaxHP;
        static const glm::vec3 k_defPlayerPosition;
        static const glm::vec3 k_defMainHandPosition;

        static GameObject * gameObject;
        static SpatialComponent * spatial;
        static NewtonianComponent * newtonian;
        static CapsuleBounderComponent * bounder;
        static CameraComponent * camera;
        static PlayerControllerComponent * controller;
        static PlayerComponent * playerComp;
        static HealthComponent * health;

        static void init(const glm::vec3 & position);

    };

    //--------------------------------------------------------------------------
    // Enemies

    struct Enemies {

        struct Basic {

            static const String k_defMeshName;
            static const bool k_defIsToon;
            static const glm::vec3 k_defScale;
            static const unsigned int k_defWeight;
            static const float k_defMoveSpeed;
            static const float k_defMaxHP;

            static void create(const glm::vec3 & position);

            static void spawn();

        };

        static void enablePathfinding();

        static void disablePathfinding();

    };

    //--------------------------------------------------------------------------
    // Weapons

    struct Weapons {

        struct Grenade {

            static const String k_defMeshName;
            static const String k_defTexName;
            static const bool k_defIsToon;
            static const glm::vec3 k_defScale;
            static const unsigned int k_defWeight;
            static const float k_defSpeed; 
            static const float k_defDamage;
            static const float k_defRadius;

            static void fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel);
            static void fireFromPlayer();

        };

        static void destroyAllProjectiles();

    };

    //--------------------------------------------------------------------------
    // Freecam

    struct Freecam {

        static const float k_defMinSpeed;
        static const float k_defMaxSpeed;

        static GameObject * gameObject;
        static SpatialComponent * spatialComp;
        static CameraComponent * cameraComp;
        static CameraControllerComponent * controllerComp;

        static void init();

    };
    
    //--------------------------------------------------------------------------

    public:

    static void init();

    static void update(float dt);

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

    // Main/ImGui helper functions
    public:

    static const glm::vec3 & lightDir() { return Lighting:: }

};

#endif