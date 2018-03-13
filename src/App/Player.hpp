#pragma once



#include "Game.hpp"



namespace player {

    constexpr float k_height = 1.75f;
    constexpr float k_width = k_height / 4.0f;
    constexpr unsigned int k_weight = 5;
    constexpr float k_lookSpeed = 0.005f;
    constexpr float k_moveSpeed = 5.0f;
    constexpr float k_sprintSpeed = 15.0f;
    constexpr float k_jumpSpeed = 5.0f;
    constexpr float k_fov = 45.0f;
    constexpr float k_near = 0.1f;
    constexpr float k_far = 300.0f;

    extern const glm::vec3 k_mainHandPosition;

    extern GameObject * gameObject;
    extern SpatialComponent * spatial;
    extern NewtonianComponent * newtonian;
    extern CapsuleBounderComponent * bounder;
    extern CameraComponent * camera;
    extern PlayerControllerComponent * controller;

    void setup(const glm::vec3 & position);

}