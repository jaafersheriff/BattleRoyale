#pragma once

#include "glm/glm.hpp"

#include "Component/Component.hpp"

#include "Positionable.hpp"
#include "Scaleable.hpp"
#include "Orientable.hpp"



class SpatialSystem;



class SpatialComponent : public Component, public Positionable, public Scaleable, public Orientable {

    friend Scene;
    friend SpatialSystem;

    protected: // only scene or friends can create component

    SpatialComponent(GameObject & gameObject);
    SpatialComponent(GameObject & gameObject, const glm::vec3 & position);
    SpatialComponent(GameObject & gameObject, const glm::vec3 & position, const glm::vec3 & scale);
    SpatialComponent(GameObject & gameObject, const glm::vec3 & position, const glm::vec3 & scale, const glm::mat3 & orient);
    SpatialComponent(GameObject & gameObject, const glm::vec3 & position, const glm::vec3 & scale, const glm::quat & orient);

    public:

    SpatialComponent(SpatialComponent && other) = default;

    virtual SystemID systemID() const override { return SystemID::spatial; };

    public:

    virtual void update(float dt) override;

    // sets the absolute position
    void setPosition(const glm::vec3 & pos, bool silently = false);

    // moves current position by delta
    void move(const glm::vec3 & delta, bool silently = false);

    // sets the absolute scale
    void setScale(const glm::vec3 & scale, bool silently = false);

    // multiplies current scale by factor
    void scaleBy(const glm::vec3 & factor, bool silently = false);

    // sets the absolute orientation
    void setOrientation(const glm::mat3 & orient, bool silently = false);
    void setOrientation(const glm::quat & orient, bool silently = false);
    
    // rotates current orientation
    void rotate(const glm::mat3 & rot, bool silently = false);
    void rotate(const glm::quat & rot, bool silently = false);

    // set the orthonormal basis vectors
    void setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w, bool silently = false);

    public:

    const glm::mat4 & modelMatrix() const;
    const glm::mat4 & prevModelMatrix() const;
    glm::mat4 modelMatrix(float interpP) const;

    const glm::mat3 & normalMatrix() const;
    const glm::mat3 & prevNormalMatrix() const;
    glm::mat3 normalMatrix(float interpP) const;

    bool isChange() const { return Positionable::isChange() || Scaleable::isChange() || Orientable::isChange(); }

    // This is not a true velocity, only a rough estimation based on just one frame
    // Use NewtonianComponent for "physics" velocity
    glm::vec3 effectiveVelocity() const;

    private:

    mutable glm::mat4 m_modelMatrix;
    mutable glm::mat4 m_prevModelMatrix;
    mutable glm::mat3 m_normalMatrix;
    mutable glm::mat3 m_prevNormalMatrix;
    mutable bool m_modelMatrixValid;
    mutable bool m_prevModelMatrixValid;
    mutable bool m_normalMatrixValid;
    mutable bool m_prevNormalMatrixValid;
    float m_dt;

};