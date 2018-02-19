#include "CameraComponent.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Util/Util.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "IO/Window.hpp"
#include "Scene/Scene.hpp"
#include "System/RenderSystem.hpp"
#include "GameObject/Message.hpp"



CameraComponent::CameraComponent(float fov) :
    Component(),
    Orientable(),
    m_spatial(nullptr),
    m_theta(0.0f),
    m_phi(glm::pi<float>() * 0.5f),
    m_fov(fov),
    m_viewMat(),
    m_projMat(),
    m_viewMatValid(false),
    m_projMatValid(false),
    m_frustumValid(false)
{}

void CameraComponent::init(GameObject & gameObject) {
    m_gameObject = &gameObject;
    if (!(m_spatial = m_gameObject->getComponentByType<SpatialComponent>())) assert(false);
    setUVW(m_spatial->u(), m_spatial->v(), m_spatial->w());
    m_theta = 0.0f;
    m_phi = glm::pi<float>() * 0.5f;
    m_viewMatValid = false;
    m_projMatValid = false;
    m_frustumValid = false;

    auto spatTransformCallback([&](const Message & msg_) {
        m_viewMatValid = false;
        m_frustumValid = false;
    });
    auto spatRotationCallback([&](const Message & msg_) {
        m_viewMatValid = false;
        m_frustumValid = false;
        detUVW();
    });
    Scene::get().addReceiver<SpatialPositionSetMessage>(getGameObject(), spatTransformCallback);
    Scene::get().addReceiver<SpatialMovedMessage>(getGameObject(), spatTransformCallback);
    Scene::get().addReceiver<SpatialScaleSetMessage>(getGameObject(), spatTransformCallback);
    Scene::get().addReceiver<SpatialScaledMessage>(getGameObject(), spatTransformCallback);
    Scene::get().addReceiver<SpatialOrientationSetMessage>(getGameObject(), spatRotationCallback);
    Scene::get().addReceiver<SpatialRotatedMessage>(getGameObject(), spatRotationCallback);
    Scene::get().addReceiver<CollisionAdjustMessage>(getGameObject(), spatTransformCallback); // necessary as collision sets position silently

    auto windowSizeCallback([&] (const Message & msg_) {
        m_projMatValid = false;
        m_frustumValid = false;
    });
    Scene::get().addReceiver<WindowSizeMessage>(nullptr, windowSizeCallback);

    auto nearFarCallback([&] (const Message & msg_) {
        m_projMatValid = false;
        m_frustumValid = false;
    });
    Scene::get().addReceiver<NearFarMessage>(nullptr, nearFarCallback);
}

void CameraComponent::update(float dt) {

}

void CameraComponent::lookAt(const glm::vec3 & p) {
    lookInDir(p - m_spatial->position());
}

void CameraComponent::lookInDir(const glm::vec3 & dir) {
    glm::vec3 relativeDir = Util::mapTo(m_spatial->u(), m_spatial->v(), m_spatial->w()) * dir;
    glm::vec3 spherical(Util::cartesianToSpherical(glm::vec3(-relativeDir.z, -relativeDir.x, relativeDir.y)));
    m_theta = spherical.y;
    m_phi = spherical.z;
    detUVW();
    m_viewMatValid = false;
    m_frustumValid = false;
}

void CameraComponent::angle(float theta, float phi, bool relative, bool silently) {
    if (relative) {
        m_theta += theta;
        m_phi += phi;
    }
    else {
        m_theta = theta;
        m_phi = phi;
    }
    if      (m_theta >  glm::pi<float>()) m_theta = std::fmod(m_theta, glm::pi<float>()) - glm::pi<float>();
    else if (m_theta < -glm::pi<float>()) m_theta = glm::pi<float>() - std::fmod(-m_theta, glm::pi<float>());
    if (m_phi < 0.0f) m_phi = 0.0f;
    if (m_phi > glm::pi<float>()) m_phi = glm::pi<float>();

    detUVW();
    m_viewMatValid = false;
    m_frustumValid = false;

    if (!silently) Scene::get().sendMessage<CameraRotatedMessage>(m_gameObject, *this);
}

void CameraComponent::setFOV(float fov) {
    m_fov = fov;
    m_projMatValid = false;
    m_frustumValid = false;
}

glm::vec3 CameraComponent::getLookDir() const {
    return -w();
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

const glm::mat4 & CameraComponent::getView() const {
    if (!m_viewMatValid) detView();
    return m_viewMat;
}

const glm::mat4 & CameraComponent::getProj() const {
    if (!m_projMatValid) detProj();
    return m_projMat;
}

void CameraComponent::detUVW() {
    glm::vec3 w(-Util::sphericalToCartesian(1.0f, m_theta, m_phi));
    w = glm::vec3(-w.y, w.z, -w.x); // one of the many reasons I like z to be up
    glm::vec3 v(Util::sphericalToCartesian(1.0f, m_theta, m_phi - glm::pi<float>() * 0.5f));
    v = glm::vec3(-v.y, v.z, -v.x);
    glm::vec3 u(glm::cross(v, w));

    // adjust relative to orientation of base
    const glm::mat3 & orient(m_spatial->orientationMatrix());
    setUVW(orient * u, orient * v, orient * w);
}

void CameraComponent::detView() const {
    m_viewMat = Util::viewMatrix(m_spatial->position(), u(), v(), w());
    m_viewMatValid = true;
}

void CameraComponent::detProj() const {
    m_projMat = glm::perspective(m_fov, Window::getAspectRatio(), RenderSystem::get().near(), RenderSystem::get().far());
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
