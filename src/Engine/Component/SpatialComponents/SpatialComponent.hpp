#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"



class SpatialSystem;



class SpatialComponent : public Component {

    friend Scene;
    friend SpatialSystem;

    protected: // only scene can create component

    SpatialComponent();
    SpatialComponent(const glm::vec3 & position, const glm::vec3 & scale);
    SpatialComponent(const glm::vec3 & position, const glm::vec3 & scale, const glm::mat3 & rotation);

    public:

    virtual SystemID systemID() const override { return SystemID::spatial; };

    virtual void update(float dt) override;

    public:

    // sets the absolute position
    void setPosition(const glm::vec3 & pos);

    // moves current position by delta
    void move(const glm::vec3 & delta);

    // sets the absolute scale
    void setScale(const glm::vec3 & scale);

    // multiplies current scale by factor
    void scale(const glm::vec3 & factor);

    // sets the absolute rotation
    void setRotation(const glm::mat3 & rot);
    
    // rotates current rotation by mat
    void rotate(const glm::mat3 & mat);

    // set the orthonormal basis vectors
    void setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w);

    public:

    const glm::vec3 & position() const { return m_position; }
    const glm::vec3 & scale() const { return m_scale; }
    const glm::vec3 & u() const { return m_u; }
    const glm::vec3 & v() const { return m_v; }
    const glm::vec3 & w() const { return m_w; }

    const glm::mat3 & rotationMatrix() const;
    const glm::mat4 & modelMatrix() const;
    const glm::mat3 & normalMatrix() const;

    private:

    void detRotationMatrix() const;
    void detModelMatrix() const;
    void detNormalMatrix() const;

    private:

    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_u, m_v, m_w; // orthonormal basis vectors

    mutable glm::mat3 m_rotationMatrix;
    mutable glm::mat4 m_modelMatrix;
    mutable glm::mat3 m_normalMatrix;
    mutable bool m_rotationMatrixValid;
    mutable bool m_modelMatrixValid;
    mutable bool m_normalMatrixValid;

};