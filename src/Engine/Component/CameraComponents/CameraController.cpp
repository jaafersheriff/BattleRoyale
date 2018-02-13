#include "CameraController.hpp"

#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

// Debug
#include <stdio.h>

void CameraController::init() {

}

void CameraController::update(float dt) {
    /* Checks if the camera direction or location has been modified */
    bool modified;

    if (Mouse::isDown(GLFW_MOUSE_BUTTON_1)) {
        lookAround(dt);
        modified = true;
    }

    if (Keyboard::isKeyPressed(front)) {
        moveFront(dt);
        modified = true;
    }
    if (Keyboard::isKeyPressed(back)) {
        moveBack(dt);
        modified = true;
    }
    if (Keyboard::isKeyPressed(left)) {
        moveLeft(dt);
        modified = true;
    }
    if (Keyboard::isKeyPressed(right)) {
        moveRight(dt);
        modified = true;
    }
    if (Keyboard::isKeyPressed(up)) {
        moveUp(dt);
        modified = true;
    }
    if (Keyboard::isKeyPressed(down)) {
        moveDown(dt);
        modified = true;
    }

    /* If the camera has been modified, then update the view frustum */ 
    /* if (modified) {
        printf("Camera has been modified\n");
    } */
}
void CameraController::lookAround(float dt) {
    camera->theta += Mouse::dx * lookSpeed * dt;
    camera->phi -= Mouse::dy * lookSpeed * dt;
}

void CameraController::moveFront(float dt) {
    gameObject->getSpatial()->move(camera->w * moveSpeed * dt);
    camera->lookAt += camera->w * moveSpeed * dt;
}

void CameraController::moveBack(float dt) {
    gameObject->getSpatial()->move(-(camera->w * moveSpeed * dt));
    camera->lookAt -= camera->w * moveSpeed * dt;
}

void CameraController::moveRight(float dt) {
    gameObject->getSpatial()->move(camera->u * moveSpeed * dt);
    camera->lookAt += camera->u * moveSpeed * dt;
}

void CameraController::moveLeft(float dt) {
    gameObject->getSpatial()->move(-(camera->u * moveSpeed * dt));
    camera->lookAt -= camera->u * moveSpeed * dt;
}

void CameraController::moveUp(float dt) {
    gameObject->getSpatial()->move(camera->v * moveSpeed * dt);
    camera->lookAt += camera->v * moveSpeed * dt;
}

void CameraController::moveDown(float dt) {
    gameObject->getSpatial()->move(-(camera->v * moveSpeed * dt));
    camera->lookAt -= camera->v * moveSpeed * dt;
}

