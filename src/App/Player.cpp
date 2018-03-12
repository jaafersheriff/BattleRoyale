#include "Player.hpp"



namespace player {

namespace { // File static stuff

    GameObject * f_gameObject;
    SpatialComponent * f_spatialComp;
    NewtonianComponent * f_newtonianComp;
    CapsuleBounderComponent * f_bounderComp;
    CameraComponent * f_cameraComp;
    PlayerControllerComponent * f_controllerComp;

}

    void setup(const glm::vec3 & position) {
        f_gameObject = &Scene::createGameObject();
        f_spatialComp = &Scene::addComponent<SpatialComponent>(*f_gameObject, position);
        f_newtonianComp = &Scene::addComponent<NewtonianComponent>(*f_gameObject);
        Scene::addComponentAs<GravityComponent, AcceleratorComponent>(*f_gameObject);
        Scene::addComponent<GroundComponent>(*f_gameObject);
        Capsule playerCap(glm::vec3(0.0f, -k_defHeight * 0.5f + k_defWidth, 0.0f), k_defWidth, k_defHeight - 2.0f * k_defWidth);
        f_bounderComp = &Scene::addComponentAs<CapsuleBounderComponent, BounderComponent>(*f_gameObject, 5, playerCap);
        f_cameraComp = &Scene::addComponent<CameraComponent>(*f_gameObject, k_defFOV, k_defNear, k_defFar);
        f_controllerComp = &Scene::addComponent<PlayerControllerComponent>(*f_gameObject, k_defLookSpeed, k_defMoveSpeed, k_defJumpSpeed, k_defSprintSpeed);

        // An example of using object initialization message
        auto initCallback([&](const Message & msg) {            
            f_cameraComp->lookInDir(f_cameraComp->getLookDir());
        });
        Scene::addReceiver<ObjectInitMessage>(f_gameObject, initCallback);
    }

    const GameObject & gameObject() {
        return *f_gameObject;
    }

    const SpatialComponent & spatial() {
        return *f_spatialComp;
    }

    const CameraComponent & camera() {
        return *f_cameraComp;
    }

    PlayerControllerComponent & controller() {
        return *f_controllerComp;
    }

}