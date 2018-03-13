#include "Player.hpp"



namespace player {

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
        Capsule playerCap(glm::vec3(0.0f, -k_defHeight * 0.5f + k_defWidth, 0.0f), k_defWidth, k_defHeight - 2.0f * k_defWidth);
        bounder = &Scene::addComponentAs<CapsuleBounderComponent, BounderComponent>(*gameObject, k_weight, playerCap);
        camera = &Scene::addComponent<CameraComponent>(*gameObject, k_defFOV, k_defNear, k_defFar);
        controller = &Scene::addComponent<PlayerControllerComponent>(*gameObject, k_defLookSpeed, k_defMoveSpeed, k_defJumpSpeed, k_defSprintSpeed);

        // An example of using object initialization message
        auto initCallback([&](const Message & msg) {            
            camera->lookInDir(camera->getLookDir());
        });
        Scene::addReceiver<ObjectInitMessage>(gameObject, initCallback);
    }

}