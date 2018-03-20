#include "CameraComponent.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Util/Util.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "IO/Window.hpp"
#include "Scene/Scene.hpp"
#include "System/RenderSystem.hpp"
#include "GameObject/Message.hpp"



CameraComponent::CameraComponent(GameObject & gameObject, float fov, float near, float far, SpatialComponent * spatial) :
    Component(gameObject),
    m_spatial(spatial),
    m_fov(fov),
    m_near(near),
    m_far(far),
    m_isOrtho(false),
    m_hBounds(),
    m_vBounds(),
    m_viewMat(),
    m_projMat(),
    m_viewMatValid(false),
    m_projMatValid(false),
    m_frustumValid(false)
{}

CameraComponent::CameraComponent(GameObject & gameObject, glm::vec2 horiz, glm::vec2 vert, float near, float far, SpatialComponent * spatial) :
    Component(gameObject),
    m_spatial(spatial),
    m_fov(0.0f),
    m_near(near),
    m_far(far),
    m_isOrtho(true),
    m_hBounds(horiz),
    m_vBounds(vert),
    m_viewMat(),
    m_projMat(),
    m_viewMatValid(false),
    m_projMatValid(false),
    m_frustumValid(false)
{}


void CameraComponent::init() {
    if (m_spatial) assert(&m_spatial->gameObject() == &gameObject());
    else assert(m_spatial = gameObject().getSpatial());

    auto spatChangeCallback([&](const Message & msg_) {
        m_viewMatValid = false;
        m_frustumValid = false;
    });
    Scene::addReceiver<SpatialChangeMessage>(&gameObject(), spatChangeCallback);
    Scene::addReceiver<CollisionAdjustMessage>(&gameObject(), spatChangeCallback); // necessary as collision sets position silently

    if (!m_isOrtho) {
        auto windowSizeCallback([&] (const Message & msg_) {
            m_projMatValid = false;
            m_frustumValid = false;
        });
        Scene::addReceiver<WindowFrameSizeMessage>(nullptr, windowSizeCallback);
    }
}

void CameraComponent::setFOV(float fov) {
    m_fov = fov;
    m_projMatValid = false;
    m_frustumValid = false;
}

void CameraComponent::setNearFar(float near, float far) {
    m_near = near;
    m_far = far;
    m_projMatValid = false;
    m_frustumValid = false;
}

void CameraComponent::setOrthoBounds(glm::vec2 h, glm::vec2 v) {
    m_hBounds = h;
    m_vBounds = v;
    m_projMatValid = false;
    m_frustumValid = false;
}

namespace {

float distToPlane(const glm::vec4 & plane, const glm::vec3 & p) {
    return plane.x * p.x + plane.y * p.y + plane.z * p.z + plane.w;
}

}

const bool CameraComponent::sphereInFrustum(const Sphere & sphere) const {
    if (!m_frustumValid) detFrustum();

    if (distToPlane(m_frustumLeft,   sphere.origin) < -sphere.radius) return false;
    if (distToPlane(m_frustumRight,  sphere.origin) < -sphere.radius) return false;
    if (distToPlane(m_frustumBottom, sphere.origin) < -sphere.radius) return false;
    if (distToPlane(m_frustumTop,    sphere.origin) < -sphere.radius) return false;
    if (distToPlane(m_frustumNear,   sphere.origin) < -sphere.radius) return false;
    if (distToPlane(m_frustumFar,    sphere.origin) < -sphere.radius) return false;

    return true;
}

glm::vec3 CameraComponent::getLookDir() const {
    return -m_spatial->w();
};

const glm::mat4 & CameraComponent::getView() const {
    if (!m_viewMatValid) detView();
    return m_viewMat;
}

const glm::mat4 & CameraComponent::getProj() const {
    if (!m_projMatValid) detProj();
    return m_projMat;
}

void CameraComponent::detView() const {
    m_viewMat = Util::viewMatrix(m_spatial->position(), m_spatial->u(), m_spatial->v(), m_spatial->w());
    m_viewMatValid = true;
}

void CameraComponent::detProj() const {
    if (m_isOrtho) {
        m_projMat = glm::ortho(m_hBounds.x, m_hBounds.y, m_vBounds.x, m_vBounds.y, m_near, m_far);
    }
    else {
        m_projMat = glm::perspective(m_fov, Window::getAspectRatio(), m_near, m_far);
    }
    m_projMatValid = true;
}

void CameraComponent::detFrustum() const {
    /* composite matrix */
    glm::mat4 comp = getProj() * getView();

    m_frustumLeft.x = comp[0][3] + comp[0][0];
    m_frustumLeft.y = comp[1][3] + comp[1][0];
    m_frustumLeft.z = comp[2][3] + comp[2][0];
    m_frustumLeft.w = comp[3][3] + comp[3][0];
    m_frustumLeft /= glm::length(glm::vec3(m_frustumLeft));

    m_frustumRight.x = comp[0][3] - comp[0][0];
    m_frustumRight.y = comp[1][3] - comp[1][0];
    m_frustumRight.z = comp[2][3] - comp[2][0];
    m_frustumRight.w = comp[3][3] - comp[3][0];
    m_frustumRight /= glm::length(glm::vec3(m_frustumRight));

    m_frustumBottom.x = comp[0][3] + comp[0][1];
    m_frustumBottom.y = comp[1][3] + comp[1][1];
    m_frustumBottom.z = comp[2][3] + comp[2][1];
    m_frustumBottom.w = comp[3][3] + comp[3][1];
    m_frustumBottom /= glm::length(glm::vec3(m_frustumBottom));

    m_frustumTop.x = comp[0][3] - comp[0][1];
    m_frustumTop.y = comp[1][3] - comp[1][1];
    m_frustumTop.z = comp[2][3] - comp[2][1];
    m_frustumTop.w = comp[3][3] - comp[3][1];
    m_frustumTop /= glm::length(glm::vec3(m_frustumTop));

    m_frustumNear.x = comp[0][3] + comp[0][2];
    m_frustumNear.y = comp[1][3] + comp[1][2];
    m_frustumNear.z = comp[2][3] + comp[2][2];
    m_frustumNear.w = comp[3][3] + comp[3][2];
    m_frustumNear /= glm::length(glm::vec3(m_frustumNear));

    m_frustumFar.x = comp[0][3] - comp[0][2];
    m_frustumFar.y = comp[1][3] - comp[1][2];
    m_frustumFar.z = comp[2][3] - comp[2][2];
    m_frustumFar.w = comp[3][3] - comp[3][2];
    m_frustumFar /= glm::length(glm::vec3(m_frustumFar));

    m_frustumValid = true;
}
