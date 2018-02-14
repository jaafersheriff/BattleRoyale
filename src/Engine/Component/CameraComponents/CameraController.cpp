#include "CameraController.hpp"

#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

void CameraController::init() {

}

void CameraController::update(float dt) {
    if (Mouse::isDown(GLFW_MOUSE_BUTTON_1)) {
        lookAround(dt);
    }

    if (Keyboard::isKeyPressed(front)) {
        moveFront(dt);
    }
    if (Keyboard::isKeyPressed(back)) {
        moveBack(dt);
    }
    if (Keyboard::isKeyPressed(left)) {
        moveLeft(dt);
    }
    if (Keyboard::isKeyPressed(right)) {
        moveRight(dt);
    }
    if (Keyboard::isKeyPressed(up)) {
        moveUp(dt);
    }
    if (Keyboard::isKeyPressed(down)) {
        moveDown(dt);
    }
}
void CameraController::lookAround(float dt) {
    camera->theta += Mouse::dx * lookSpeed * dt;
    camera->phi -= Mouse::dy * lookSpeed * dt;
    camera->setDirty();
}

void CameraController::moveFront(float dt) {
    gameObject->getSpatial()->move(camera->w * moveSpeed * dt);
    camera->lookAt += camera->w * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::moveBack(float dt) {
    gameObject->getSpatial()->move(-(camera->w * moveSpeed * dt));
    camera->lookAt -= camera->w * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::moveRight(float dt) {
    gameObject->getSpatial()->move(camera->u * moveSpeed * dt);
    camera->lookAt += camera->u * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::moveLeft(float dt) {
    gameObject->getSpatial()->move(-(camera->u * moveSpeed * dt));
    camera->lookAt -= camera->u * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::moveUp(float dt) {
    gameObject->getSpatial()->move(camera->v * moveSpeed * dt);
    camera->lookAt += camera->v * moveSpeed * dt;
    camera->setDirty();
}

void CameraController::moveDown(float dt) {
    gameObject->getSpatial()->move(-(camera->v * moveSpeed * dt));
    camera->lookAt -= camera->v * moveSpeed * dt;
    camera->setDirty();
}
