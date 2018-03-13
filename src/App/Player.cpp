#include "Player.hpp"



namespace player {

    const glm::vec3 k_mainHandPosition = glm::vec3();//glm::vec3(k_width * 0.5f, 0.0f, -k_width * 0.5f);

    GameObject * gameObject;
    SpatialComponent * spatial;
    NewtonianComponent * newtonian;
    CapsuleBounderComponent * bounder;
    CameraComponent * camera;
    PlayerControllerComponent * controller;

    void setup(const glm::vec3 & position) {
        gameObject = &Scene::createGameObject();
        spatial = &Scene::addComponent<SpatialComponent>(*gameObject, position);
        newtonian = &Scene::addComponent<NewtonianComponent>(*gameObject);
        Scene::addComponentAs<GravityComponent, AcceleratorComponent>(*gameObject);
        Scene::addComponent<GroundComponent>(*gameObject);
        Capsule playerCap(glm::vec3(0.0f, -k_height * 0.5f + k_width, 0.0f), k_width, k_height - 2.0f * k_width);
        bounder = &Scene::addComponentAs<CapsuleBounderComponent, BounderComponent>(*gameObject, k_weight, playerCap);
        camera = &Scene::addComponent<CameraComponent>(*gameObject, k_fov, k_near, k_far);
        controller = &Scene::addComponent<PlayerControllerComponent>(*gameObject, k_lookSpeed, k_moveSpeed, k_jumpSpeed, k_sprintSpeed);

        // An example of using object initialization message
        auto initCallback([&](const Message & msg) {            
            camera->lookInDir(camera->getLookDir());
        });
        Scene::addReceiver<ObjectInitMessage>(gameObject, initCallback);
    }

}