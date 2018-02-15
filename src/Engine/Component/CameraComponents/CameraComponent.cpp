#include "CameraComponent.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Util/Util.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "IO/Window.hpp"

void CameraComponent::init() {
    this->projection = glm::perspective(fov, Window::getAspectRatio(), near, far);
    this->view = glm::lookAt(gameObject->getSpatial()->position(), lookAt, glm::vec3(0, 1, 0));
    phi = theta = 0.0;
    
    float fovRadians;
    fovRadians = glm::radians(fov);

    farPlaneHeight = 2 * glm::tan(fovRadians / 2) * far;
    farPlaneWidth = farPlaneHeight * Window::getAspectRatio();

    nearPlaneHeight = 2 * glm::tan(fovRadians / 2) * near;
    nearPlaneWidth = nearPlaneHeight * Window::getAspectRatio();

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
    bottomPlaneNormal = glm::normalize(glm::cross(u,
        bottomPlanePoint - goPos));

    leftPlanePoint = nearPlanePoint - u * nearPlaneWidth / 2.f;
    leftPlaneNormal = glm::normalize(glm::cross(leftPlanePoint -
        goPos, v));

    rightPlanePoint = nearPlanePoint + u * nearPlaneWidth / 2.f;
    rightPlaneNormal = glm::normalize(glm::cross(v,
        rightPlanePoint - goPos));
    if (isDirty || gameObject->getSpatial()->transformedFlag()) {
        this->projection = glm::perspective(fov, Window::getAspectRatio(), near, far);
        this->view = glm::lookAt(gameObject->getSpatial()->position(), lookAt, glm::vec3(0, 1, 0));
        isDirty = false;
    }
}

const bool CameraComponent::sphereInFrustum(glm::vec3 center, float radius) const {
    // TODO : Create loop or function to make this less lines of code
    // https://www.khanacademy.org/math/linear-algebra/vectors-and-spaces/dot-cross-products/v/point-distance-to-plane

    /* Temporary variables for QOL */
    float centerDist;
    glm::vec3 hypotenuse;

    /* Test if the sphere is completely in the negative side of
        the far frustum plane */
    hypotenuse = center - farPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, farPlaneNormal) /
        glm::length(hypotenuse) / 
        glm::length(farPlaneNormal);
    if (centerDist < 0 && -centerDist > radius)
        return false;

    /* Test if the sphere is completely in the negative side of
        the near frustum plane */
    hypotenuse = center - nearPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, nearPlaneNormal) /
        glm::length(hypotenuse) /
        glm::length(nearPlaneNormal);
    if (centerDist < 0 && -centerDist > radius)
        return false;
    
    /* Test if the sphere is completely in the negative side of
        the top frustum plane */
    hypotenuse = center - topPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, topPlaneNormal) /
        glm::length(hypotenuse) /
        glm::length(topPlaneNormal);
    if (centerDist < 0 && -centerDist > radius)
        return false;
    
    /* Test if the sphere is completely in the negative side of
        the bottom frustum plane */
    hypotenuse = center - bottomPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, bottomPlaneNormal) /
        glm::length(hypotenuse) /
        glm::length(bottomPlaneNormal);
    if (centerDist < 0 && -centerDist > radius)
        return false;

    /* Test if the sphere is completely in the negative side of
        the left frustum plane */
    hypotenuse = center - leftPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, leftPlaneNormal) /
        glm::length(hypotenuse) /
        glm::length(leftPlaneNormal);
    if (centerDist < 0 && -centerDist > radius)
        return false;

    /* Test if the sphere is completely in the negative side of
        the right frustum plane */
    hypotenuse = center - rightPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, rightPlaneNormal) /
        glm::length(hypotenuse) /
        glm::length(rightPlaneNormal);
    if (centerDist < 0 && -centerDist > radius)
        return false;

    return true;
}
