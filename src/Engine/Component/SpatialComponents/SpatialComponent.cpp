#include "SpatialComponent.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Scene/Scene.hpp"
#include "Util/Util.hpp"



SpatialComponent::SpatialComponent() :
    Component(),
    Orientable(),
    m_position(),
    m_scale(1.0f),
    m_modelMatrix(),
    m_normalMatrix(),
    m_modelMatrixValid(false),
    m_normalMatrixValid(false)
{}

SpatialComponent::SpatialComponent(const glm::vec3 & loc, const glm::vec3 & scale) :
    Component(),
    Orientable(),
    m_position(loc),
    m_scale(scale),
    m_modelMatrix(),
    m_normalMatrix(),
    m_modelMatrixValid(false),
    m_normalMatrixValid(false)
{}

SpatialComponent::SpatialComponent(const glm::vec3 & loc, const glm::vec3 & scale, const glm::mat3 & orientation) :
    SpatialComponent(loc, scale)
{
    setOrientation(orientation, true);
}

void SpatialComponent::init() {

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

void SpatialComponent::setOrientation(const glm::mat3 & orient, bool silently) {
    Orientable::setOrientation(orient);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::get().sendMessage<SpatialOrientationSetMessage>(getGameObject(), *this);
}

void SpatialComponent::rotate(const glm::mat3 & mat, bool silently) {
    Orientable::rotate(mat);
    m_modelMatrixValid = false;
    m_normalMatrixValid = false;
    if (!silently) Scene::get().sendMessage<SpatialRotatedMessage>(getGameObject(), *this);
}

void SpatialComponent::setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w, bool silently) {
    Orientable::setUVW(u, v, w);
    if (!silently) Scene::get().sendMessage<SpatialOrientationSetMessage>(getGameObject(), *this);
}
    
const glm::mat4 & SpatialComponent::modelMatrix() const {
    if (!m_modelMatrixValid) detModelMatrix();
    return m_modelMatrix;
}

const glm::mat3 & SpatialComponent::normalMatrix() const {
    if (!m_normalMatrixValid) detNormalMatrix();
    return m_normalMatrix;
}

void SpatialComponent::detModelMatrix() const {
    m_modelMatrix = Util::compositeTransform(m_scale, orientationMatrix(), m_position);
    m_modelMatrixValid = true;
}

void SpatialComponent::detNormalMatrix() const {
    // this is valid and waaaaaaaay faster than inverting the model matrix
    m_normalMatrix = orientationMatrix() * glm::mat3(glm::scale(glm::mat4(), 1.0f / m_scale));
    m_normalMatrixValid = true;
}