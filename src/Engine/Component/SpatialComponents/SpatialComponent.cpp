#include "SpatialComponent.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Scene/Scene.hpp"
#include "Util/Util.hpp"



SpatialComponent::SpatialComponent() :
    m_position(),
    m_scale(1.0f),
    m_u(1.0f, 0.0f, 0.0f),
    m_v(0.0f, 1.0f, 0.0f),
    m_w(0.0f, 0.0f, 1.0f),
    m_rotationMatrix(),
    m_modelMatrix(),
    m_normalMatrix(),
    m_rotationMatrixValid(false),
    m_modelMatrixValid(false),
    m_normalMatrixValid(false)
{}

SpatialComponent::SpatialComponent(const glm::vec3 & loc, const glm::vec3 & scale) :
    m_position(loc),
    m_scale(scale),
    m_u(1.0f, 0.0f, 0.0f),
    m_v(0.0f, 1.0f, 0.0f),
    m_w(0.0f, 0.0f, 1.0f),
    m_modelMatrix(),
    m_normalMatrix(),
    m_rotationMatrix(),
    m_rotationMatrixValid(false),
    m_modelMatrixValid(false),
    m_normalMatrixValid(false)
{}

SpatialComponent::SpatialComponent(const glm::vec3 & loc, const glm::vec3 & scale, const glm::mat3 & rotation) :
    SpatialComponent(loc, scale)
{
    setRotation(rotation, true);
}

void SpatialComponent::update(float dt) {
    
}

void SpatialComponent::setPosition(const glm::vec3 & loc, bool silently) {
    m_position = loc;
    m_modelMatrixValid = false;
    if (!silently) Scene::get().sendMessage<SpatialPositionSetMessage>(getGameObject(), *this);
}

void SpatialComponent::move(const glm::vec3 & delta, bool silently) {
    m_position += delta;
    m_modelMatrixValid = false;
    if (!silently) Scene::get().sendMessage<SpatialMovedMessage>(getGameObject(), *this);
}

void SpatialComponent::setScale(const glm::vec3 & scale, bool silently) {
    m_scale = scale;
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::get().sendMessage<SpatialScaleSetMessage>(getGameObject(), *this);
}

void SpatialComponent::scale(const glm::vec3 & factor, bool silently) {
    m_scale *= factor;
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::get().sendMessage<SpatialScaledMessage>(getGameObject(), *this);
}

void SpatialComponent::setRotation(const glm::mat3 & rot, bool silently) {
    m_rotationMatrix = rot;
    m_rotationMatrixValid = true;
    glm::mat3 trans(glm::transpose(m_rotationMatrix));
    m_u = trans[0];
    m_v = trans[1];
    m_w = trans[2];
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::get().sendMessage<SpatialRotationSetMessage>(getGameObject(), *this);
}

void SpatialComponent::rotate(const glm::mat3 & mat, bool silently) {
    m_rotationMatrix = mat * m_rotationMatrix;
    m_rotationMatrixValid = true;
    glm::mat3 trans(glm::transpose(m_rotationMatrix));
    m_u = trans[0];
    m_v = trans[1];
    m_w = trans[2];
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::get().sendMessage<SpatialRotatedMessage>(getGameObject(), *this);
}

void SpatialComponent::setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w, bool silently) {
    m_u = u;
    m_v = v;
    m_w = w;
    if (!silently) Scene::get().sendMessage<SpatialRotationSetMessage>(getGameObject(), *this);
}

const glm::mat3 & SpatialComponent::rotationMatrix() const {
    if (!m_rotationMatrixValid) detRotationMatrix();
    return m_rotationMatrix;
}
    
const glm::mat4 & SpatialComponent::modelMatrix() const {
    if (!m_modelMatrixValid) detModelMatrix();
    return m_modelMatrix;
}

const glm::mat3 & SpatialComponent::normalMatrix() const {
    if (!m_normalMatrixValid) detNormalMatrix();
    return m_normalMatrix;
}

void SpatialComponent::detRotationMatrix() const {
    m_rotationMatrix = Util::mapTo(m_u, m_v, m_w);
    m_rotationMatrixValid = true;
}

void SpatialComponent::detModelMatrix() const {
    m_modelMatrix = Util::compositeTransform(m_scale, rotationMatrix(), m_position);
    m_modelMatrixValid = true;
}

void SpatialComponent::detNormalMatrix() const {
    // this is valid and waaaaaaaay faster than inverting the model matrix
    m_normalMatrix = rotationMatrix() * glm::mat3(glm::scale(glm::mat4(), 1.0f / m_scale));
    m_normalMatrixValid = true;
}