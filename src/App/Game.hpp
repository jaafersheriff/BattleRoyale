#pragma once



#include "EngineApp/EngineApp.hpp"



namespace freecam {

    extern GameObject * gameObject;
    extern SpatialComponent * spatialComp;
    extern CameraComponent * cameraComp;
    extern CameraControllerComponent * controllerComp;

    void setup();

}



extern const glm::vec3 k_defGravity;

void startGame();