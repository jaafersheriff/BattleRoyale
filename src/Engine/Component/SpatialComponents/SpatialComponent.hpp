#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"



class SpatialSystem;



class SpatialComponent : public Component {

    friend Scene;
    friend SpatialSystem;

    protected: // only scene can create component

    SpatialComponent();
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

    public:

    const glm::vec3 & position() const { return m_position; }
    const glm::vec3 & scale() const { return m_scale; }
    const glm::mat3 & rotation() const { return m_rotation; }

    const glm::mat4 & modelMatrix() const;
    const glm::mat3 & normalMatrix() const;

    private:

    void detModelMatrix() const;
    void detNormalMatrix() const;

    private:

    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::mat3 m_rotation;

    mutable glm::mat4 m_modelMatrix;
    mutable glm::mat3 m_normalMatrix;
    mutable bool m_modelMatrixValid;
    mutable bool m_normalMatrixValid;

};