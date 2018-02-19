#include "CameraComponent.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Util/Util.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "IO/Window.hpp"



CameraComponent::CameraComponent(float fov, float near, float far) :
    m_u(0.0f, 0.0f, 1.0f),
    m_v(0.0f, 1.0f, 0.0f),
    m_w(-1.0f, 0.0f, 0.0f),
    m_theta(0.0f),
    m_phi(glm::pi<float>() * 0.5f),
    m_fov(fov),
    m_near(near),
    m_far(far),
    m_viewMat(),
    m_projMat(),
    m_viewMatValid(false),
    m_projMatValid(false)
{}

void CameraComponent::init() {    
    float fovRadians;
    fovRadians = glm::radians(m_fov);
    fovRadians *= 1.2f;

    float planeScale;
    planeScale = 1.2f;

    farPlaneHeight = 2 * glm::tan(fovRadians / 2) * m_far * planeScale;
    farPlaneWidth = farPlaneHeight * Window::getAspectRatio();

    nearPlaneHeight = 2 * glm::tan(fovRadians / 2) * m_near * planeScale;
    nearPlaneWidth = nearPlaneHeight * Window::getAspectRatio();

    update(0.0f);
}

void CameraComponent::update(float dt) {
    /* Local variable for QOL */
    glm::vec3 goPos;
    goPos = gameObject->getSpatial()->position();

    /* update matrices */
    if (gameObject->getSpatial()->transformedFlag()) {
        detView();
    }

    /* Update view frustum data */

    /* w = forwards-backwards of camera */
    farPlanePoint = goPos - m_w * m_far;
    farPlaneNormal = m_w;

    nearPlanePoint = goPos - m_w * m_near;
    nearPlaneNormal = -m_w;

    /* v = up-down of camera */
    /* u = left-right of camera */
    topPlanePoint = nearPlanePoint + m_v * nearPlaneHeight / 2.f;
    topPlaneNormal = glm::normalize(glm::cross(topPlanePoint -
        goPos, m_u));

    bottomPlanePoint = nearPlanePoint - m_v * nearPlaneHeight / 2.f;
    bottomPlaneNormal = glm::normalize(glm::cross(m_u,
        bottomPlanePoint - goPos));

    leftPlanePoint = nearPlanePoint - m_u * nearPlaneWidth / 2.f;
    leftPlaneNormal = glm::normalize(glm::cross(leftPlanePoint -
        goPos, m_v));

    rightPlanePoint = nearPlanePoint + m_u * nearPlaneWidth / 2.f;
    rightPlaneNormal = glm::normalize(glm::cross(m_v,
        rightPlanePoint - goPos));
}

void CameraComponent::lookAt(const glm::vec3 & p) {
    lookInDir(p - gameObject->getSpatial()->position());
}

void CameraComponent::lookInDir(const glm::vec3 & dir) {
    glm::vec3 spherical(Util::cartesianToSpherical(glm::vec3(dir.x, -dir.z, dir.y)));
    m_theta = spherical.y;
    m_phi = spherical.z;
    detUVW();
}

void CameraComponent::angle(float yaw, float pitch, bool relative) {
    if (relative) {
        m_theta -= yaw;
        m_phi -= pitch;
    }
    else {
        m_theta = -yaw;
        m_phi = glm::pi<float>() * 0.5f - pitch;
    }
    if      (m_theta > glm::pi<float>()) m_theta -= glm::pi<float>() * 2.0f;
    else if (m_theta < -glm::pi<float>()) m_theta += glm::pi<float>() * 2.0f;
    if (m_phi < 0.0f) m_phi = 0.0f;
    if (m_phi > glm::pi<float>()) m_phi = glm::pi<float>();

    detUVW();
}

const bool CameraComponent::sphereInFrustum(const Sphere & sphere) const {
    if (glm::dot(sphere.origin -    farPlanePoint,    farPlaneNormal) <= -sphere.radius) return false;
    if (glm::dot(sphere.origin -   nearPlanePoint,   nearPlaneNormal) <= -sphere.radius) return false;
    if (glm::dot(sphere.origin -    topPlanePoint,    topPlaneNormal) <= -sphere.radius) return false;
    if (glm::dot(sphere.origin - bottomPlanePoint, bottomPlaneNormal) <= -sphere.radius) return false;
    if (glm::dot(sphere.origin -   leftPlanePoint,   leftPlaneNormal) <= -sphere.radius) return false;
    if (glm::dot(sphere.origin -  rightPlanePoint,  rightPlaneNormal) <= -sphere.radius) return false;

    return true;

    /*

    // TODO : Create loop or function to make this less lines of code
    // https://www.khanacademy.org/math/linear-algebra/vectors-and-spaces/dot-cross-products/v/point-distance-to-plane

    // Temporary variables for QOL
    float centerDist;
    glm::vec3 hypotenuse;
    
    // Test if the sphere is completely in the negative side of the far frustum plane
    hypotenuse = sphere.origin - farPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, farPlaneNormal) /
        glm::length(hypotenuse) / 
        glm::length(farPlaneNormal);
    if (centerDist < 0 && -centerDist > sphere.radius)
        return false;

    // Test if the sphere is completely in the negative side of the near frustum plane
    hypotenuse = sphere.origin - nearPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, nearPlaneNormal) /
        glm::length(hypotenuse) /
        glm::length(nearPlaneNormal);
    if (centerDist < 0 && -centerDist > sphere.radius)
        return false;
    
    // Test if the sphere is completely in the negative side of the top frustum plane
    hypotenuse = sphere.origin - topPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, topPlaneNormal) /
        glm::length(hypotenuse) /
        glm::length(topPlaneNormal);
    if (centerDist < 0 && -centerDist > sphere.radius)
        return false;
    
    // Test if the sphere is completely in the negative side of the bottom frustum plane
    hypotenuse = sphere.origin - bottomPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, bottomPlaneNormal) /
        glm::length(hypotenuse) /
        glm::length(bottomPlaneNormal);
    if (centerDist < 0 && -centerDist > sphere.radius)
        return false;

    // Test if the sphere is completely in the negative side of the left frustum plane
    hypotenuse = sphere.origin - leftPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, leftPlaneNormal) /
        glm::length(hypotenuse) /
        glm::length(leftPlaneNormal);
    if (centerDist < 0 && -centerDist > sphere.radius)
        return false;

    // Test if the sphere is completely in the negative side of the right frustum plane
    hypotenuse = sphere.origin - rightPlanePoint;
    centerDist = glm::length(hypotenuse) *
        glm::dot(hypotenuse, rightPlaneNormal) /
        glm::length(hypotenuse) /
        glm::length(rightPlaneNormal);
    if (centerDist < 0 && -centerDist > sphere.radius)
        return false;

    return true;

    */
}

const glm::mat4 & CameraComponent::getView() const {
    if (!m_viewMatValid) detView();
    return m_viewMat;
}

const glm::mat4 & CameraComponent::getProj() const {
    if (!m_projMatValid) detProj();
    return m_projMat;
}

void CameraComponent::detUVW() {    
    m_w = -Util::sphericalToCartesian(1.0f, m_theta, m_phi);
    m_w = glm::vec3(m_w.x, m_w.z, -m_w.y); // one of the many reasons I like z to be up
    m_v = Util::sphericalToCartesian(1.0f, m_theta, m_phi - glm::pi<float>() * 0.5f);
    m_v = glm::vec3(m_v.x, m_v.z, -m_v.y);
    m_u = glm::cross(m_v, m_w);
}

void CameraComponent::detView() const {
    const glm::vec3 & pos(gameObject->getSpatial()->position());
    m_viewMat = glm::lookAt(pos, pos - m_w, glm::vec3(0.0f, 1.0f, 0.0f));

}

void CameraComponent::detProj() const {
    m_projMat = glm::perspective(m_fov, Window::getAspectRatio(), m_near, m_far);
}
