#include "SpatialComponent.hpp"
#include "System/SpatialSystem.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Scene/Scene.hpp"
#include "Util/Util.hpp"



SpatialComponent::SpatialComponent(GameObject & gameObject) :
    Component(gameObject),
    Positionable(),
    Scaleable(),
    Orientable(),
    m_modelMatrix(),
    m_prevModelMatrix(m_modelMatrix),
    m_normalMatrix(),
    m_prevNormalMatrix(m_normalMatrix),
    m_modelMatrixValid(true),
    m_prevModelMatrixValid(true),
    m_normalMatrixValid(true),
    m_prevNormalMatrixValid(true),
    m_dt(Util::infinity())
{}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & position) :
    SpatialComponent(gameObject)
{
    setPosition(position, true);
}

SpatialComponent::SpatialComponent(GameObject & gameObject, const glm::vec3 & position, const glm::vec3 & scale) :
    SpatialComponent(gameObject, position)
{
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
    m_dt = dt;
    if (Positionable::isChange()) {
        Positionable::update();
        m_modelMatrixValid = false;
    }
    if (Scaleable::isChange()) {
        Scaleable::update();
        m_modelMatrixValid = false;
        m_normalMatrixValid = false;
    }
    if (Orientable::isChange()) {
        Orientable::update();
        m_modelMatrixValid = false;
        m_normalMatrixValid = false;
    }
    m_prevModelMatrixValid = false;
    m_prevNormalMatrixValid = false;
}

void SpatialComponent::setPosition(const glm::vec3 & position, bool silently) {
    Positionable::setPosition(position);
    m_modelMatrixValid = false;
    m_prevModelMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialPositionSetMessage>(&gameObject(), *this);
}

void SpatialComponent::move(const glm::vec3 & delta, bool silently) {
    Positionable::move(delta);
    m_modelMatrixValid = !Positionable::isChange();
    m_prevModelMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialMovedMessage>(&gameObject(), *this);
}

void SpatialComponent::setScale(const glm::vec3 & scale, bool silently) {
    Scaleable::setScale(scale);
    m_modelMatrixValid = false;
    m_prevModelMatrixValid = false;
    m_normalMatrixValid = false;
    m_prevNormalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialScaleSetMessage>(&gameObject(), *this);
}

void SpatialComponent::scaleBy(const glm::vec3 & factor, bool silently) {
    Scaleable::scale(factor);
    m_modelMatrixValid = !Scaleable::isChange();
    m_prevModelMatrixValid = false;
    m_normalMatrixValid = !Scaleable::isChange();
    m_prevNormalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialScaledMessage>(&gameObject(), *this);
}

void SpatialComponent::setOrientation(const glm::mat3 & orient, bool silently) {
    Orientable::setOrientation(orient);
    m_modelMatrixValid = false;
    m_prevModelMatrixValid = false;
    m_normalMatrixValid = false;
    m_prevNormalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialOrientationSetMessage>(&gameObject(), *this);
}

void SpatialComponent::setOrientation(const glm::quat & orient, bool silently) {
    Orientable::setOrientation(orient);
    m_modelMatrixValid = false;
    m_prevModelMatrixValid = false;
    m_normalMatrixValid = false;
    m_prevNormalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialOrientationSetMessage>(&gameObject(), *this);
}

void SpatialComponent::rotate(const glm::mat3 & rot, bool silently) {
    Orientable::rotate(rot);
    m_modelMatrixValid = !Orientable::isChange();
    m_prevModelMatrixValid = false;
    m_normalMatrixValid = !Orientable::isChange();
    m_prevNormalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialRotatedMessage>(&gameObject(), *this);
}

void SpatialComponent::rotate(const glm::quat & rot, bool silently) {
    Orientable::rotate(rot);
    m_modelMatrixValid = !Orientable::isChange();
    m_prevModelMatrixValid = false;
    m_normalMatrixValid = !Orientable::isChange();
    m_prevNormalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialRotatedMessage>(&gameObject(), *this);
}

void SpatialComponent::setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w, bool silently) {
    Orientable::setUVW(u, v, w);
    m_modelMatrixValid = false;
    m_prevModelMatrixValid = false;
    m_normalMatrixValid = false;
    m_prevNormalMatrixValid = false;
    if (!silently) Scene::sendMessage<SpatialOrientationSetMessage>(&gameObject(), *this);
}

const glm::mat4 & SpatialComponent::modelMatrix() const {
    if (!m_modelMatrixValid) {
        m_modelMatrix = Util::compositeTransform(scale(), orientMatrix(), position());
        m_modelMatrixValid = true;
    }
    return m_modelMatrix;
}
    
const glm::mat4 & SpatialComponent::prevModelMatrix() const {
    if (!m_prevModelMatrixValid) {
        m_prevModelMatrix = Util::compositeTransform(prevScale(), prevOrientMatrix(), prevPosition());
        m_prevModelMatrixValid = true;
    }
    return m_prevModelMatrix;
}

glm::mat4 SpatialComponent::modelMatrix(float interpP) const {
    if (Positionable::isChange() || Scaleable::isChange() || Orientable::isChange()) {
        return Util::compositeTransform(scale(interpP), orientMatrix(interpP), position(interpP));
    }
    else {
        return modelMatrix();
    }
}

const glm::mat3 & SpatialComponent::normalMatrix() const {
    if (!m_normalMatrixValid) {        
        // this is valid and waaaaaaaay faster than inverting the model matrix
        m_normalMatrix = orientMatrix() * glm::mat3(glm::scale(glm::mat4(), 1.0f / scale()));
        m_normalMatrixValid = true;
    }
    return m_normalMatrix;
}

const glm::mat3 & SpatialComponent::prevNormalMatrix() const {
    if (!m_prevNormalMatrixValid) {        
        // this is valid and waaaaaaaay faster than inverting the model matrix
        m_prevNormalMatrix = prevOrientMatrix() * glm::mat3(glm::scale(glm::mat4(), 1.0f / prevScale()));
        m_prevNormalMatrixValid = true;
    }
    return m_prevNormalMatrix;
}

glm::mat3 SpatialComponent::normalMatrix(float interpP) const {
    if (Scaleable::isChange() || Orientable::isChange()) {
        return orientMatrix(interpP) * glm::mat3(glm::scale(glm::mat4(), 1.0f / scale(interpP)));
    }
    else {
        return normalMatrix();
    }
}

glm::vec3 SpatialComponent::effectiveVelocity() const {
    return (position() - prevPosition()) / m_dt;
}