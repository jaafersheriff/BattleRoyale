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
    Orientable(),
    m_spatial(spatial),
    m_theta(0.0f),
    m_phi(glm::pi<float>() * 0.5f),
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
    Orientable(),
    m_spatial(spatial),
    m_theta(0.0f),
    m_phi(glm::pi<float>() * 0.5f),
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

    setUVW(m_spatial->u(), m_spatial->v(), m_spatial->w());
    m_theta = 0.0f;
    m_phi = glm::pi<float>() * 0.5f;
    m_viewMatValid = false;
    m_projMatValid = false;
    m_frustumValid = false;

    auto spatChangeCallback([&](const Message & msg_) {
        m_viewMatValid = false;
        m_frustumValid = false;
        detUVW();
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

    if (!silently) Scene::sendMessage<CameraRotatedMessage>(&gameObject(), *this);
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
    const glm::mat3 & orient(m_spatial->orientMatrix());
    setUVW(orient * u, orient * v, orient * w);
}

void CameraComponent::detView() const {
    m_viewMat = Util::viewMatrix(m_spatial->position(), u(), v(), w());
    m_viewMatValid = true;
}

void CameraComponent::detProj() const {
    if (m_isOrtho) {
        /* Emulate glm::ortho without translation */
        float width = m_hBounds.y - m_hBounds.x;
        float height = m_vBounds.y - m_vBounds.x;
        float length = m_far - m_near;
        m_projMat = glm::mat4(1.f);
        m_projMat[0][0] = 2.f / width;
        m_projMat[1][1] = 2.f / height;
        m_projMat[2][2] = -2.f / length;
        m_projMat[3][3] = 1.f;
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
