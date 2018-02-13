#include "CameraComponent.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Util/Util.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

// Debug
#include <stdio.h>

void CameraComponent::init() {
    this->projection = glm::perspective(fov, aspect, near, far);
    phi = theta = 0.0;
    
    float fovRadians;
    fovRadians = glm::radians(fov);

    farPlaneHeight = 2 * glm::tan(fovRadians / 2) * far;
    farPlaneWidth = farPlaneHeight * aspect;

    nearPlaneHeight = 2 * glm::tan(fovRadians / 2) * near;
    nearPlaneWidth = nearPlaneHeight * aspect;

    update(0.f);
}

void CameraComponent::update(float dt) {
    /* Local variable for QOL */
    glm::vec3 goPos;
    goPos = gameObject->getSpatial()->position();

    /* Update basis vectors */
    w = glm::normalize(lookAt - goPos);
    u = glm::normalize(glm::cross(w, glm::vec3(0, 1, 0)));
    v = glm::normalize(glm::cross(u, w));

    /* Update look at */
    glm::vec3 sphere(
        glm::cos(phi)*glm::cos(theta),
        glm::sin(phi),
        glm::cos(phi)*glm::cos((Util::PI / 2.f) - theta));
    lookAt = goPos + glm::normalize(sphere);

    /* Update view frustum data */

    /* w = forwards-backwards of camera */
    farPlanePoint = goPos + w * far;
    farPlaneNormal = -w;

    nearPlanePoint = goPos + w * near;
    nearPlaneNormal = w;

    /* v = up-down of camera */
    /* u = left-right of camera */
    topPlanePoint = nearPlanePoint + v * nearPlaneHeight / 2.f;
    topPlaneNormal = glm::normalize(glm::cross(topPlanePoint -
        goPos, u));

    bottomPlanePoint = nearPlanePoint - v * nearPlaneHeight / 2.f;
    bottomPlaneNormal = glm::normalize(glm::cross(bottomPlanePoint -
        goPos, u));

    leftPlanePoint = nearPlanePoint - u * nearPlaneWidth / 2.f;
    leftPlaneNormal = glm::normalize(glm::cross(leftPlanePoint -
        goPos, v));

    rightPlanePoint = nearPlanePoint + u * nearPlaneWidth / 2.f;
    rightPlaneNormal = glm::normalize(glm::cross(rightPlanePoint -
        goPos, v));

    /* Update view matrix */
    this->projection = glm::perspective(fov, aspect, near, far);
    this->view = glm::lookAt(goPos, lookAt, glm::vec3(0, 1, 0));
}
