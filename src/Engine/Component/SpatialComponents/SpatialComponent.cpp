#include "SpatialComponent.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Scene/Scene.hpp"
#include "Util/Util.hpp"



SpatialComponent::SpatialComponent(GameObject & gameObject) :
    Component(gameObject),
    Orientable(),
    m_position(),
    m_prevPosition(m_position),
    m_scale(1.0f),
    m_prevScale(m_scale),
    m_isPositionChange(false),
    m_isScaleChange(false),
    m_modelMatrix(),
    m_normalMatrix(),
    m_modelMatrixValid(false),
    m_normalMatrixValid(false)
{}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & position, const glm::vec3 & scale) :
    SpatialComponent(gameObject)
{
    setPosition(position, true);
    setScale(scale, true);
}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & position, const glm::vec3 & scale, const glm::mat3 & orient) :
    SpatialComponent(gameObject, position, scale)
{
    setOrientation(orient, true);
}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & position, const glm::vec3 & scale, const glm::quat & orient) :
    SpatialComponent(gameObject, position, scale)
{
    setOrientation(orient, true);
}

void SpatialComponent::update(float dt) {
    Orientable::update();
    m_prevPosition = m_position;
    m_isPositionChange = false;
    m_prevScale = m_scale;
    m_isScaleChange = false;
}

void SpatialComponent::setPosition(const glm::vec3 & position, bool silently) {
    m_position = position;
    m_prevPosition = m_position;
    m_isPositionChange = false;
    m_modelMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialPositionSetMessage>(&gameObject(), *this);
}

void SpatialComponent::move(const glm::vec3 & delta, bool silently) {
    m_position += delta;
    m_isPositionChange = delta != glm::vec3();
    m_modelMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialMovedMessage>(&gameObject(), *this);
}

void SpatialComponent::setScale(const glm::vec3 & scale, bool silently) {
    m_scale = scale;
    m_prevScale = m_scale;
    m_isScaleChange = false;
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialScaleSetMessage>(&gameObject(), *this);
}

void SpatialComponent::scale(const glm::vec3 & factor, bool silently) {
    m_scale *= factor;
    m_isScaleChange = factor != glm::vec3(1.0f);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialScaledMessage>(&gameObject(), *this);
}

void SpatialComponent::setOrientation(const glm::mat3 & orient, bool silently) {
    Orientable::setOrientation(orient);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialOrientationSetMessage>(&gameObject(), *this);
}

void SpatialComponent::setOrientation(const glm::quat & orient, bool silently) {
    Orientable::setOrientation(orient);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialOrientationSetMessage>(&gameObject(), *this);
}

void SpatialComponent::rotate(const glm::mat3 & rot, bool silently) {
    Orientable::rotate(rot);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialRotatedMessage>(&gameObject(), *this);
}

void SpatialComponent::rotate(const glm::quat & rot, bool silently) {
    Orientable::rotate(rot);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialRotatedMessage>(&gameObject(), *this);
}

void SpatialComponent::setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w, bool silently) {
    Orientable::setUVW(u, v, w);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialOrientationSetMessage>(&gameObject(), *this);
}

glm::vec3 SpatialComponent::position(float interpP) const {
    return m_isPositionChange ? glm::mix(m_prevPosition, m_position, interpP) : m_position;
}

glm::vec3 SpatialComponent::scale(float interpP) const {
    return m_isScaleChange ? glm::mix(m_prevScale, m_scale, interpP) : m_scale;
}
    
const glm::mat4 & SpatialComponent::modelMatrix() const {
    if (!m_modelMatrixValid) detModelMatrix();
    return m_modelMatrix;
}

glm::mat4 SpatialComponent::modelMatrix(float interpP) const {
    if (m_isPositionChange || m_isScaleChange || Orientable::isChange()) {
        return Util::compositeTransform(scale(interpP), orientMatrix(interpP), position(interpP));
    }
    else {
        return modelMatrix();
    }
}

const glm::mat3 & SpatialComponent::normalMatrix() const {
    if (!m_normalMatrixValid) detNormalMatrix();
    return m_normalMatrix;
}

glm::mat3 SpatialComponent::normalMatrix(float interpP) const {
    if (m_isScaleChange || Orientable::isChange()) {
        return orientMatrix(interpP) * glm::mat3(glm::scale(glm::mat4(), 1.0f / scale(interpP)));
    }
    else {
        return normalMatrix();
    }
}

void SpatialComponent::detModelMatrix() const {
    m_modelMatrix = Util::compositeTransform(m_scale, orientMatrix(), m_position);
    m_modelMatrixValid = true;
}

void SpatialComponent::detNormalMatrix() const {
    // this is valid and waaaaaaaay faster than inverting the model matrix
    m_normalMatrix = orientMatrix() * glm::mat3(glm::scale(glm::mat4(), 1.0f / m_scale));
    m_normalMatrixValid = true;
}